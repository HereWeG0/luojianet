/**
 * Copyright 2021, 2022 LuoJiaNET Research and Development Group, Wuhan University
 * Copyright 2021, 2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "minddata/dataset/text/vocab.h"

#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <algorithm>

#include "utils/file_utils.h"
#ifndef ENABLE_ANDROID
#include "utils/log_adapter.h"
#else
#include "luojianet_ms/lite/src/common/log_adapter.h"
#endif

namespace luojianet_ms {
namespace dataset {
Vocab::Vocab(std::unordered_map<WordType, WordIdType> word2id) { word2id_ = std::move(word2id); }

WordIdType Vocab::Lookup(const WordType &word) const {
  auto itr = word2id_.find(word);
  return itr == word2id_.end() ? kNoTokenExists : itr->second;
}

std::vector<WordIdType> Vocab::Lookup(const std::vector<WordType> &words) const {
  std::vector<WordIdType> ids;
  std::transform(words.begin(), words.end(), std::back_inserter(ids), [this](auto w) { return Lookup(w); });
  return ids;
}

WordType Vocab::ReverseLookup(const WordIdType &id) {
  // lazy initialization, since I think it's not common use but waste memory
  if (id2word_.empty()) {
    for (const auto [word_, id_] : word2id_) {
      id2word_[id_] = word_;
    }
  }
  auto itr = id2word_.find(id);
  return itr == id2word_.end() ? kNoIdExists : itr->second;
}

std::vector<WordType> Vocab::ReverseLookup(const std::vector<WordIdType> &ids) {
  // lazy initialization, since I think it's not common use but waste memory
  if (id2word_.empty()) {
    for (const auto [word_, id_] : word2id_) {
      id2word_[id_] = word_;
    }
  }
  std::vector<WordType> words;
  std::transform(ids.begin(), ids.end(), std::back_inserter(words), [this](auto i) { return ReverseLookup(i); });
  return words;
}

#ifdef ENABLE_PYTHON
Status Vocab::BuildFromPyList(const py::list &words, const py::list &special_tokens, bool prepend_special,
                              std::shared_ptr<Vocab> *vocab) {
  if (vocab == nullptr) {
    RETURN_STATUS_UNEXPECTED("Vocab::BuildFromPyList: input vocab can not be null");
  }
  // check of duplication on both words and special_tokens will be performed in python
  // special_tokens and words both need to be unique, and shouldn't overlap
  std::unordered_map<WordType, WordIdType> word2id;
  // if special is added in front, normal words id will start from number of special tokens
  WordIdType word_id = prepend_special ? static_cast<WordIdType>(special_tokens.size()) : 0;

  for (auto word : words) {
    word2id[py::str(word)] = word_id++;
  }

  word_id = prepend_special ? 0 : word2id.size();

  for (auto special_token : special_tokens) {
    word2id[py::str(special_token)] = word_id++;
  }

  *vocab = std::make_shared<Vocab>(std::move(word2id));
  return Status::OK();
}

Status Vocab::BuildFromPyDict(const py::dict &words, std::shared_ptr<Vocab> *vocab) {
  if (vocab == nullptr) {
    RETURN_STATUS_UNEXPECTED("Vocab::BuildFromPyDict: input vocab can not be null");
  }
  std::unordered_map<WordType, WordIdType> word2id;
  for (auto p : words) {
    word2id[py::str(p.first)] = py::reinterpret_borrow<py::int_>(p.second);
  }
  *vocab = std::make_shared<Vocab>(std::move(word2id));
  return Status::OK();
}
#endif

void Vocab::append_word(const std::string &word) {
  if (word2id_.find(word) == word2id_.end()) {
    word2id_[word] = word2id_.size();
  }
}

Status Vocab::BuildFromUnorderedMap(const std::unordered_map<WordType, WordIdType> &words,
                                    std::shared_ptr<Vocab> *vocab) {
  if (vocab == nullptr) {
    RETURN_STATUS_UNEXPECTED("Vocab::BuildFromUnorderedMap: input vocab can not be null");
  }
  // Validate parameters and build map
  std::unordered_map<WordType, WordIdType> word2id;
  for (auto p : words) {
    if (p.second < 0) {
      RETURN_STATUS_UNEXPECTED("from_dict: index can not be negetive, but got " + std::to_string(p.second));
    }
    word2id[p.first] = p.second;
  }
  *vocab = std::make_shared<Vocab>(std::move(word2id));
  return Status::OK();
}

Status Vocab::BuildFromVector(const std::vector<WordType> &words, const std::vector<WordType> &special_tokens,
                              bool prepend_special, std::shared_ptr<Vocab> *vocab) {
  if (vocab == nullptr) {
    RETURN_STATUS_UNEXPECTED("Vocab::BuildFromVector: input vocab can not be null");
  }
  std::unordered_map<WordType, WordIdType> word2id;

  // if special is added in front, normal words id will start from number of special tokens
  WordIdType word_id = prepend_special ? static_cast<WordIdType>(special_tokens.size()) : 0;
  for (auto word : words) {
    if (word2id.find(word) != word2id.end()) {
      RETURN_STATUS_UNEXPECTED("from_list: word_list contains duplicate word: " + word + ".");
    }
    word2id[word] = word_id++;
  }

  word_id = prepend_special ? 0 : word2id.size();

  for (auto special_token : special_tokens) {
    if (word2id.find(special_token) != word2id.end()) {
      RETURN_STATUS_UNEXPECTED(
        "from_list: "
        "special_tokens and word_list contain duplicate word: " +
        special_token + ".");
    }
    word2id[special_token] = word_id++;
  }

  *vocab = std::make_shared<Vocab>(std::move(word2id));
  return Status::OK();
}

Status Vocab::BuildFromFileCpp(const std::string &path, const std::string &delimiter, int32_t vocab_size,
                               const std::vector<WordType> &special_tokens, bool prepend_special,
                               std::shared_ptr<Vocab> *vocab) {
  if (vocab == nullptr) {
    RETURN_STATUS_UNEXPECTED("Vocab::BuildFromFileCpp: input vocab can not be null");
  }
  // Validate parameters
  auto realpath = FileUtils::GetRealPath(path.data());
  CHECK_FAIL_RETURN_UNEXPECTED(realpath.has_value(), "Get real path failed, path=" + path);

  CHECK_FAIL_RETURN_UNEXPECTED(
    !(vocab_size < 0 && vocab_size != -1),
    "from_file: vocab_size should be either -1 or positive integer, but got: " + std::to_string(vocab_size));

  std::string duplicate_sp;
  for (const WordType &sp : special_tokens) {
    if (std::count(special_tokens.begin(), special_tokens.end(), sp) > 1) {
      if (duplicate_sp.find(sp) == std::string::npos) {
        duplicate_sp = duplicate_sp.empty() ? duplicate_sp + sp : duplicate_sp + ", " + sp;
      }
    }
  }
  CHECK_FAIL_RETURN_UNEXPECTED(duplicate_sp.empty(),
                               "from_file: special_tokens contains duplicate word: " + duplicate_sp);

  std::unordered_set<std::string> specials;
  // used to check that words in file don't contain any special token that already exists
  for (auto word : special_tokens) {
    specials.insert(word);
  }
  WordIdType word_id = prepend_special ? static_cast<WordIdType>(special_tokens.size()) : 0;
  std::unordered_map<WordType, WordIdType> word2id;

  std::fstream handle(realpath.value(), std::ios::in);
  CHECK_FAIL_RETURN_UNEXPECTED(handle.good() && handle.is_open(), "from_file: fail to open: " + realpath.value());

  std::string word;
  while (std::getline(handle, word)) {
    if (!delimiter.empty()) {
      // if delimiter is not found, find_first_of would return std::string::npos which is -1
      word = word.substr(0, word.find_first_of(delimiter));
    }
    CHECK_FAIL_RETURN_UNEXPECTED(word2id.find(word) == word2id.end(),
                                 "from_file: word_list contains duplicate word:" + word);
    CHECK_FAIL_RETURN_UNEXPECTED(specials.find(word) == specials.end(),
                                 "from_file: special_tokens and word_list contain duplicate word:" + word);

    word2id[word] = word_id++;
    // break if enough row is read, if vocab_size is smaller than 0
    if (word2id.size() == vocab_size) break;
  }

  word_id = prepend_special ? 0 : word2id.size();

  for (auto special_token : special_tokens) {
    word2id[special_token] = word_id++;
  }

  *vocab = std::make_shared<Vocab>(std::move(word2id));
  return Status::OK();
}

Status Vocab::BuildFromFile(const std::string &path, const std::string &delimiter, int32_t vocab_size,
                            const py::list &special_tokens, bool prepend_special, std::shared_ptr<Vocab> *vocab) {
  if (vocab == nullptr) {
    RETURN_STATUS_UNEXPECTED("Vocab::BuildFromFile: input vocab can not be null");
  }
  // python validator checks special_tokens doesn't contain any duplicate words
  std::unordered_set<std::string> specials;
  // used to check that words in file don't contain any special token that already exists
  for (auto word : special_tokens) {
    specials.insert(py::str(word));
  }
  WordIdType word_id = prepend_special ? static_cast<WordIdType>(special_tokens.size()) : 0;
  std::unordered_map<WordType, WordIdType> word2id;

  auto realpath = FileUtils::GetRealPath(path.data());
  if (!realpath.has_value()) {
    RETURN_STATUS_UNEXPECTED("Get real path failed, path=" + path);
  }

  std::fstream handle(realpath.value(), std::ios::in);
  CHECK_FAIL_RETURN_UNEXPECTED(handle.good() && handle.is_open(), "from_file: fail to open:" + path);
  std::string word;
  while (std::getline(handle, word)) {
    if (!delimiter.empty()) {
      // if delimiter is not found, find_first_of would return std::string::npos which is -1
      word = word.substr(0, word.find_first_of(delimiter));
    }
    CHECK_FAIL_RETURN_UNEXPECTED(word2id.find(word) == word2id.end(), "from_file: duplicate word:" + word + ".");
    CHECK_FAIL_RETURN_UNEXPECTED(specials.find(word) == specials.end(),
                                 "from_file: " + word + " is already in special_tokens.");
    word2id[word] = word_id++;
    // break if enough row is read, if vocab_size is smaller than 0
    if (word2id.size() == vocab_size) break;
  }

  word_id = prepend_special ? 0 : word2id.size();

  for (auto special_token : special_tokens) {
    word2id[py::str(special_token)] = word_id++;
  }

  *vocab = std::make_shared<Vocab>(std::move(word2id));
  return Status::OK();
}

const WordIdType Vocab::kNoTokenExists = -1;
const WordType Vocab::kNoIdExists = std::string();

}  // namespace dataset
}  // namespace luojianet_ms
