#PACK_NAME, PACK_ROOT, PACK_VERSION and PACK_CONFIG_DIR are predefined

if(NOT TARGET glm)
	find_dependency(glm CONFIG)
endif()