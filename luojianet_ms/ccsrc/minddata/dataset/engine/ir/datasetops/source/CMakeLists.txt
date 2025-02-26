file(GLOB_RECURSE _CURRENT_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cc")
set_property(SOURCE ${_CURRENT_SRC_FILES} PROPERTY COMPILE_DEFINITIONS SUBMODULE_ID=luojianet_ms::SubModuleId::SM_MD)
add_subdirectory(samplers)

set(DATASET_ENGINE_IR_DATASETOPS_SOURCE_SRC_FILES
        ag_news_node.cc
        album_node.cc
        celeba_node.cc
        cifar100_node.cc
        cifar10_node.cc
        cityscapes_node.cc
        clue_node.cc
        coco_node.cc
        conll2000_node.cc
        csv_node.cc
        dbpedia_node.cc
        div2k_node.cc
        emnist_node.cc
        fake_image_node.cc
        fashion_mnist_node.cc
        flickr_node.cc
        image_folder_node.cc
        iwslt2016_node.cc
        iwslt2017_node.cc
        kmnist_node.cc
        lj_speech_node.cc
        manifest_node.cc
        minddata_node.cc
        mnist_node.cc
        penn_treebank_node.cc
        photo_tour_node.cc
        places365_node.cc
        qmnist_node.cc
        random_node.cc
        sbu_node.cc
        sogou_news_node.cc
        speech_commands_node.cc
        stl10_node.cc
        tedlium_node.cc
        text_file_node.cc
        tf_record_node.cc
        udpos_node.cc
        usps_node.cc
        voc_node.cc
        yahoo_answers_node.cc
        yelp_review_node.cc
        yes_no_node.cc
        )

if(ENABLE_PYTHON)
    set(DATASET_ENGINE_IR_DATASETOPS_SOURCE_SRC_FILES
            ${DATASET_ENGINE_IR_DATASETOPS_SOURCE_SRC_FILES}
            generator_node.cc
            )
endif()

add_library(engine-ir-datasetops-source OBJECT ${DATASET_ENGINE_IR_DATASETOPS_SOURCE_SRC_FILES})
