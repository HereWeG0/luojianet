file(GLOB_RECURSE BUILDER_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "op_lazy_builder.cc")

set_property(SOURCE ${BUILDER_SRC_LIST} PROPERTY COMPILE_DEFINITIONS SUBMODULE_ID=luojianet_ms::SubModuleId::SM_DEVICE)
add_library(_luojianet_runtime_op_builder_obj OBJECT ${BUILDER_SRC_LIST})
