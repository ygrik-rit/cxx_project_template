# https://github.com/BlankSpruce/gersemi/blob/master/extension-example/as_module/gersemi_extension_example/__init__.py

from gersemi.builtin_commands import builtin_commands

modified_target_sources = builtin_commands["target_sources"]
modified_target_sources["keyword_preprocessors"] = {
    key: "sort+unique" for key in modified_target_sources["multi_value_keywords"]
}

modified_target_link_libraries = builtin_commands["target_link_libraries"]
modified_target_link_libraries["keyword_preprocessors"] = {
    key: "sort+unique" for key in modified_target_link_libraries["multi_value_keywords"]
}


command_definitions = {
    "target_sources": modified_target_sources,
    "target_link_libraries": modified_target_link_libraries,
}