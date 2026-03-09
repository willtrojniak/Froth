# 03/08/2026
Accomplished:
- Shader reflection to determine descriptor sets
- Merging descriptor sets over shaders to inform pipeline

Next Steps:
- Unit tests for merging descriptor sets
- Reflect push constants
- Hot reload shaders on file change
    - Recompile shaders
    - Reconstruct pipelines
- Refactor with greater use of `std::expected`
- Move descriptor set allocation to internal (?)

# 03/02/2026
Accomplished:
- Moved compilation of shaders to injected compiler
    - Compiler is injected by the renderer
- Ownership of resource handles moved from resources to resource manager
    - Handles are typed for better type safety
- Resource Manager polls to detect changes in latest write to underlying files
    - Verifies changes with hashes before marking resources as dirty

Next Steps:
- Hot reload shaders on file change
    - Recompile shaders
    - Reconstruct pipelines
- Refactor with greater use of `std::expected`

# 03/01/2026

Accomplished:
- Shaders are automatically compiled from glsl to spir-v

Next Steps:
- Move compilation logic out of shader module manager
- Hot reload shaders on file change
