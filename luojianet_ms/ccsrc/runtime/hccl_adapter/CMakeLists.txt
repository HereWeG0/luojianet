file(GLOB HCCL_ADAPTER_SRC_LIST ./*.cc)
set_property(SOURCE ${HCCL_ADAPTER_SRC_LIST} PROPERTY COMPILE_DEFINITIONS
  SUBMODULE_ID=luojianet_ms::SubModuleId::SM_HCCL_ADPT)
if(ENABLE_D)
    add_library(_luojianet_runtime_hccl_adapter_obj OBJECT ${HCCL_ADAPTER_SRC_LIST})
    target_include_directories(_luojianet_runtime_hccl_adapter_obj PRIVATE ${CMAKE_BINARY_DIR}/proto/ge)
    add_dependencies(_luojianet_runtime_hccl_adapter_obj graph)
    add_subdirectory(plugin)
endif()