cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
message("================begin compile protoc and create new *.pb.cc/*.pb.h=====================")
message("++++++++++++++++if you first execute this step, git fetch is slowly+++++++++++++++")
if (NOT TARGET protobuf::libprotobuf)
    include(FetchContent)
    message(CHECK_START "Fetching Protobuf")
    FetchContent_Declare(
            protobuf
            GIT_REPOSITORY "git@gitlab.alibaba-inc.com:yankai.yan/mirror_protobuf.git"
            GIT_TAG v3.15.0
            SOURCE_SUBDIR cmake)
    FetchContent_MakeAvailable(protobuf)
    message(CHECK_PASS "fetched")
endif()

file(GLOB_RECURSE proto_files ${base_path}/modules/proto/*.proto)
foreach(fil ${proto_files})
    get_filename_component(abs_fil ${fil} ABSOLUTE)
    get_filename_component(fil_we ${fil} NAME_WE)
    get_filename_component(file_dir ${fil} PATH)

    list(APPEND srcs_var "${base_path}/modules/proto/${fil_we}.pb.cc")
    list(APPEND hdrs_var "${base_path}/modules/proto/${fil_we}.pb.h")

    add_custom_command(
            OUTPUT "${base_path}/modules/proto/${fil_we}.pb.cc"
            "${base_path}/modules/proto/${fil_we}.pb.h"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMAND protobuf::protoc -I${file_dir}/ --cpp_out=${file_dir} ${abs_fil}
            DEPENDS ${fil} protobuf::protoc
            COMMENT "Running C++ protocol buffer compiler on ${fil}" VERBATIM )
endforeach()
set_source_files_properties(${srcs_var} ${hdrs_var} PROPERTIES GENERATED TRUE)
add_custom_target(generate_proto ALL
        DEPENDS ${srcs_var} ${hdrs_var} protobuf::protoc
        COMMENT "generate message target"
        VERBATIM
        )
