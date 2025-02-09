/*!
 * @file
 * @brief This file contains forward declarations, structures and constants.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

//#define MAKE_STUDENT_RELEASE

uint32_t const maxAttributes = 4;///< maximum number of vertex/fragment attributes

/**
 * @brief This structure represents 2D memory of image.
 * 2D images are used as textures and color or depth buffers of framebuffers.
 * Pixel [x,y] starts at uint8_t*ptr = ((uint8_t*)data) + y*pitch + x*bytesPerPixel;
 * channelTypes is swizzling table for colors (RGB,BGR,...)
 * for format == FLOAT32:
 *   bytesPerPixel = sizeof(float) * channels
 * for format == UINT8:
 *   bytesPerPixel = sizeof(uint8_t) * channels
 *
 * pitch = width * bytesPerPixel
 */
//! [Image]
struct Image{
  /**
   * @brief Channel type
   */
  enum Channel{
    RED   = 0,
    GREEN    ,
    BLUE     ,
    ALPHA    ,
  };
  /**
   * @brief Format of data
   */
  enum Format{
    UINT8  ,
    FLOAT32,
  };
  void*    data            = nullptr               ; ///< point to image data
  uint32_t channels        = 4                     ; ///< number of color channels
  Format   format          = UINT8                 ; ///< type of data for every channel
  Channel  channelTypes[4] = {RED,GREEN,BLUE,ALPHA}; ///< only for colors - what colors the channels represent
  uint32_t pitch           = 0                     ; ///< size of row in bytes 
  uint32_t bytesPerPixel   = 0                     ; ///< size of pixel in bytes
};
//! [Image]

/**
 * @brief This struct represent a texture
 */
//! [Texture]
struct Texture{
  Image    img       ; ///< image
  uint32_t width  = 0; ///< width of the texture
  uint32_t height = 0; ///< height of the texture
};
//! [Texture]


/**
 * @brief This enum represents vertex/fragment attribute type.
 */
//! [AttributeType]
enum class AttributeType : uint8_t{
  EMPTY =   0, ///< disabled attribute
  FLOAT =   1, ///< 1x 32-bit float
  VEC2  =   2, ///< 2x 32-bit floats
  VEC3  =   3, ///< 3x 32-bit floats
  VEC4  =   4, ///< 4x 32-bit floats
  UINT  = 8+1, ///< 1x 32-bit unsigned int
  UVEC2 = 8+2, ///< 2x 32-bit unsigned int
  UVEC3 = 8+3, ///< 3x 32-bit unsigned int
  UVEC4 = 8+4, ///< 4x 32-bit unsigned int
};
//! [AttributeType]

/**
 * @brief This union represents one vertex/fragment attribute
 */
//! [Attribute]
union Attribute{
  Attribute():v4(glm::vec4(1.f)){}
  float      v1; ///< single float
  glm::vec2  v2; ///< vector of two floats
  glm::vec3  v3; ///< vector of three floats
  glm::vec4  v4; ///< vector of four floats
  uint32_t   u1; ///< single unsigned int
  glm::uvec2 u2; ///< vector of two unsigned ints
  glm::uvec3 u3; ///< vector of three unsigned ints
  glm::uvec4 u4; ///< vector of four unsigned ints
};
//! [Attribute]

/**
 * @brief This struct represents input vertex of vertex shader.
 */
//! [InVertex]
struct InVertex{
  Attribute attributes[maxAttributes]    ; ///< vertex attributes
  uint32_t  gl_VertexID               = 0; ///< vertex id
};
//! [InVertex]

/**
 * @brief This struct represents output vertex of vertex shader.
 */
//! [OutVertex]
struct OutVertex{
  Attribute attributes[maxAttributes]                     ; ///< vertex attributes
  glm::vec4 gl_Position               = glm::vec4(0,0,0,1); ///< clip space position
};
//! [OutVertex]

/**
 * @brief This struct represents input fragment.
 */
//! [InFragment]
struct InFragment{
  Attribute attributes[maxAttributes]               ; ///< fragment attributes
  glm::vec4 gl_FragCoord              = glm::vec4(1); ///< fragment coordinates
};
//! [InFragment]

/**
 * @brief This struct represents output fragment.
 */
//! [OutFragment]
struct OutFragment{
  glm::vec4 gl_FragColor = glm::vec4(0.f); ///< fragment color
  bool      discard      = false         ; ///< discard fragment
};
//! [OutFragment]

/**
 * @brief This union represents one uniform variable.
 */
//! [Uniform]
union Uniform{
  Uniform(){}
  float      v1; ///< single float
  glm::vec2  v2; ///< two    floats
  glm::vec3  v3; ///< three  floats
  glm::vec4  v4; ///< four   floats
  uint32_t   u1; ///< single 32bit unsigned int
  glm::uvec2 u2; ///< two    32bit unsigned ints
  glm::uvec3 u3; ///< three  32bit unsigned ints
  glm::uvec4 u4; ///< four   32bit unsigned ints
  int32_t    i1; ///< single 32bit int
  glm::ivec2 i2; ///< two    32bit ints
  glm::ivec3 i3; ///< three  32bit ints
  glm::ivec4 i4; ///< four   32bit ints
  glm::mat4  m4 = glm::mat4(1.f); ///< 4x4 float matrix
};
//! [Uniform]

/**
 * @brief This enum represents index type
 */
//! [IndexType]
enum class IndexType : uint8_t{
  UINT8  = 1, ///< uin8_t type
  UINT16 = 2, ///< uin16_t type
  UINT32 = 4, ///< uint32_t type
};
//! [IndexType]


/**
 * @brief This enum represents constant shader interface common for all shaders.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [ShaderInterface]
struct ShaderInterface{
  Uniform const*uniforms  = nullptr; ///< uniform variables
  Texture const*textures  = nullptr; ///< textures
  uint32_t      gl_DrawID = 0      ; ///< draw id
};
//! [ShaderInterface]

/**
 * @brief Function type for vertex shader
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [VertexShader]
using VertexShader = void(*)(
    OutVertex            &outVertex,
    InVertex        const&inVertex ,
    ShaderInterface const&si       );
//! [VertexShader]

/**
 * @brief Function type for fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment 
 * @param uniforms uniform variables
 */
//! [FragmentShader]
using FragmentShader = void(*)(
    OutFragment          &outFragment,
    InFragment      const&inFragment ,
    ShaderInterface const&si         );
//! [FragmentShader]

/**
 * @brief This struct describes location of one vertex attribute.
 */
//! [VertexAttrib]
struct VertexAttrib{
  int32_t       bufferID = -1                  ;///< buffer id
  uint64_t      stride   = 0                   ;///< stride in bytes
  uint64_t      offset   = 0                   ;///< offset in bytes
  AttributeType type     = AttributeType::EMPTY;///< type of attribute
};
//! [VertexAttrib]

/**
 * @brief This structure represents setting for vertex pulller (vertex assembly) unit.
 * VertexArrays holds setting for reading vertices from buffers.
 */
//! [VertexArray]
struct VertexArray{
  VertexAttrib vertexAttrib[maxAttributes];       ///< settings for vertex attributes
  int32_t      indexBufferID = -1;                ///< id of index buffer
  uint64_t     indexOffset   = 0 ;                ///< offset of indices
  IndexType    indexType     = IndexType::UINT32; ///< type of indices
};
//! [VertexArray]

/**
 * @brief This structu represents a program.
 * Vertex Shader is executed on every InVertex.
 * Fragment Shader is executed on every rasterized InFragment.
 */
//! [Program]
struct Program{
  VertexShader   vertexShader   = nullptr; ///< vertex shader
  FragmentShader fragmentShader = nullptr; ///< fragment shader
  AttributeType  vs2fs[maxAttributes] = {AttributeType::EMPTY}; ///< which attributes are interpolated from vertex shader to fragment shader
};
//! [Program]


/**
 * @brief This function gets pointer to the start of the pixel in the image
 *
 * @param image image
 * @param x x coordinates
 * @param y y coordinates
 *
 * @return pointer to the start of the pixel
 */
inline void*getPixel(Image&image,uint32_t x,uint32_t y){
  auto ptr = (uint8_t*)image.data;
  ptr += image.pitch*y + image.bytesPerPixel*x;
  return ptr;
}

/**
 * @brief This function gets constant pointer to the start of the pixel in the image
 *
 * @param image image
 * @param x x coordinates
 * @param y y coordinates
 *
 * @return constant pointer to the start of the pixel
 */
inline void const*getPixel(Image const&image,uint32_t x,uint32_t y){
  auto ptr = (uint8_t*)image.data;
  ptr += image.pitch*y + image.bytesPerPixel*x;
  return ptr;
}

/**
 * @brief This structure represent a framebuffer
 * A framebuffer is used as an output of rendering
 */
//! [Framebuffer]
struct Framebuffer{
  Image    color            ; ///< color buffer
  Image    depth            ; ///< depth buffer
  uint32_t width     = 0    ; ///< width of frame buffer
  uint32_t height    = 0    ; ///< height of frame buffer
  bool     yReversed = false; ///< is y axis flipped?
};
//! [Framebuffer]

/**
 * @brief This structure represents a buffer on GPU
 * Buffer is a linear memory.
 */
//! [Buffer]
struct Buffer{
  void const* data = nullptr; ///< pointer to data
  uint64_t    size = 0      ; ///< size of data in bytes
};
//! [Buffer]

/**
 * @brief This structure represents memory on GPU
 * A GPU memory has a lot of memory types ranging from buffers, textures,
 * to uniforms programs, vertex arrays and framebuffers.
 */
//! [GPUMemory]
struct GPUMemory{
  uint32_t     maxUniforms          = 0      ; ///< maximal number of uniforms
  uint32_t     maxVertexArrays      = 0      ; ///< maximal number of vertex arrays
  uint32_t     maxTextures          = 0      ; ///< maximal number of textures
  uint32_t     maxBuffers           = 0      ; ///< maximal number of buffers
  uint32_t     maxPrograms          = 0      ; ///< maximal number of programs
  uint32_t     maxFramebuffers      = 0      ; ///< maximal number of framebuffers
  uint32_t     defaultFramebuffer   = 0      ; ///< id of default framebuffer
  Buffer      *buffers              = nullptr; ///< array of all buffers
  Texture     *textures             = nullptr; ///< array of all textures
  Uniform     *uniforms             = nullptr; ///< array of all uniform variables
  Program     *programs             = nullptr; ///< array of all programs
  Framebuffer *framebuffers         = nullptr; ///< framebuffer - output of rendering, 0 is default framebuffer
  VertexArray *vertexArrays         = nullptr; ///< array of all vertex arrays
  uint32_t     activatedFramebuffer = 0      ; ///< id of activated framebuffer
  uint32_t     activatedProgram     = 0      ; ///< id of activated program
  uint32_t     activatedVertexArray = 0      ; ///< id of activated vertex array
  uint32_t     gl_DrawID            = 0      ; ///< draw call id

  //Do not worry about these.
  //This is just to suppress valgrind warnings because of the large stack.
  //Otherwise everything would be placed on the stack and not on the heap.
  //I had to allocated this structure on the heap, because it is too large.
  GPUMemory();                 
  GPUMemory(GPUMemory const&o);
  ~GPUMemory();                
  GPUMemory&operator=(GPUMemory const&o);
};
//! [GPUMemory]


struct CommandBuffer;

/**
 * @brief This structure represents clear command.
 * Clear command stores data which are used by clearing operation on the GPU.
 */
//! [ClearCommand]
struct ClearCommand{
  glm::vec4   color      = glm::vec4(0); ///< color buffer will be cleared by this value
  float       depth      = 1e10        ; ///< depth buffer will be cleared by this value
  bool        clearColor = true        ; ///< is color cleaning enabled?
  bool        clearDepth = true        ; ///< is depth cleaning enabled?
};
//! [ClearCommand]

/**
 * @brief This structure represents draw command.
 * Draw command issues draw operation on the GPU.
 */
//! [DrawCommand]
struct DrawCommand{
  uint32_t    nofVertices     = 0    ; ///< number of vertices to draw
  bool        backfaceCulling = false; ///< is culling of backfacing triangles enabled?
};
//! [DrawCommand]

/**
 * @brief This structure represents setDrawId command.
 * SetDrawId command sets gl_DrawID during command buffer execution.
 */
//! [SetDrawIdCommand]
struct SetDrawIdCommand{
  uint32_t id = 0; ///< id of draw
};
//! [SetDrawIdCommand]

/**
 * @brief This structure represents bindFramebuffer command.
 * BindFramebuffer command binds framebuffer to which the rendering result is stored.
 */
//! [BindFramebufferCommand]
struct BindFramebufferCommand{
  uint32_t id = 0; ///< id of framebuffer to bind
};
//! [BindFramebufferCommand]

/**
 * @brief This structure represents bindProgram command.
 * bindProgram commands selects shader program that should be used during rendering.
 */
//! [BindProgramCommand]
struct BindProgramCommand{
  uint32_t id = 0; ///< selected shader program - id
};
//! [BindProgramCommand]

/**
 * @brief This structure represents bindVertexArray command.
 * bindVertexArray commands selects vertex array table that is used by Vertex Assembly unit.
 */
//! [BindVertexArrayCommand]
struct BindVertexArrayCommand{
  uint32_t id = 0; ///< selected vertex array - id
};
//! [BindVertexArrayCommand]

/**
 * @brief This structure represents sub command.
 * A sub command is secondary command buffer that should be executed.
 */
//! [SubCommand]
struct SubCommand{
  CommandBuffer*commandBuffer = nullptr; ///< pointer to secondary command buffer
};
//! [SubCommand]

/**
 * @brief This enum represents type of command.
 */
//! [CommandType]
enum class CommandType{
  EMPTY           , ///< empty command
  CLEAR           , ///< clear command
  SET_DRAW_ID     , ///< set drawId command
  DRAW            , ///< draw command
  BIND_FRAMEBUFFER, ///< bind framebuffer command
  BIND_PROGRAM    , ///< bind program command
  BIND_VERTEXARRAY, ///< bind vertex array command
  SUB_COMMAND     , ///< sub command
};
//! [CommandType]

/**
 * @brief This union represents command data.
 */
//! [CommandData]
union CommandData{
  CommandData():drawCommand(){}///< constructor
  ClearCommand            clearCommand          ;///< clear command data
  DrawCommand             drawCommand           ;///< draw command data
  SetDrawIdCommand        setDrawIdCommand      ;///< set drawId command
  BindFramebufferCommand  bindFramebufferCommand;///< bind framebuffer command
  BindProgramCommand      bindProgramCommand    ;///< bind program command
  BindVertexArrayCommand  bindVertexArrayCommand;///< bind vertex array command
  SubCommand              subCommand            ;///< sub command buffer
};
//! [CommandData]

/**
 * @brief This struct represents a command.
 */
//! [Command]
struct Command{
  CommandData data                     ;///< data of the command
  CommandType type = CommandType::EMPTY;///< type of the command
};
//! [Command]

/**
 * @brief This struct represents a command buffer.
 * Command buffer is used for CPU -> GPU communication.
 * Jobs are sent to GPU using this structure.
 */
//! [CommandBuffer]
struct CommandBuffer{
  uint32_t static const maxCommands           = 10000; ///< maximal number of commands
  uint32_t              nofCommands           = 0    ; ///< number of used commands in command buffer
  Command               commands[maxCommands]        ; ///< array of commands
};
//! [CommandBuffer]

/**
 * @brief This function can be used to insert clear command to command buffer.
 *
 * @param cb command buffer
 * @param color color for cleaning
 * @param depth depth for cleaning
 * @param clearColor should the color buffer be cleaned?
 * @param clearDepth should the depth buffer be cleaned?
 */
inline void pushClearCommand(
    CommandBuffer      &cb                       ,
    glm::vec4     const&color      = glm::vec4(0),
    float               depth      = 10e10       ,
    bool                clearColor = true        ,
    bool                clearDepth = true        ){
  auto&cmd=cb.commands[cb.nofCommands];
  cmd.type = CommandType::CLEAR;
  auto&c = cmd.data.clearCommand;
  c.color      = color;
  c.depth      = depth     ;
  c.clearColor = clearColor;
  c.clearDepth = clearDepth;
  cb.nofCommands++;
}

/**
 * @brief This function can be used to insert draw command into command buffer.
 *
 * @param cb command buffer
 * @param nofVertices number of vertices that should be rendered
 * @param backfaceCulling should the backface culling be enabled?
 */
inline void pushDrawCommand(
    CommandBuffer      &cb                     ,
    uint32_t            nofVertices            , 
    bool                backfaceCulling = false){
  auto&cmd=cb.commands[cb.nofCommands];
  cmd.type = CommandType::DRAW;
  auto&c = cmd.data.drawCommand;
  c.backfaceCulling = backfaceCulling;
  c.nofVertices     = nofVertices    ;
  cb.nofCommands++;
}

/**
 * @brief This function can be used to insert bindFramebuffer command into command buffer.
 *
 * @param cb command buffer
 * @param id id of framebuffer to bind
 */
inline void pushBindFramebufferCommand(
    CommandBuffer&cb,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::BIND_FRAMEBUFFER;
  cmd.data.bindFramebufferCommand.id = id;
}

/**
 * @brief This function inserts bindProgram command into command buffer.
 *
 * @param cb command buffer
 * @param id id of program
 */
inline void pushBindProgramCommand(
    CommandBuffer&cb,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::BIND_PROGRAM;
  cmd.data.bindProgramCommand.id = id;
}

/**
 * @brief This function inserts bindVertexArray command into command buffer.
 *
 * @param cb command buffer
 * @param id id of vertex array
 */
inline void pushBindVertexArrayCommand(
    CommandBuffer&cb,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::BIND_VERTEXARRAY;
  cmd.data.bindVertexArrayCommand.id = id;
}

/**
 * @brief This function inserts setDrawId command into command buffer.
 *
 * @param cb command buffer
 * @param id gl_DrawID
 */
inline void pushSetDrawIdCommand(
    CommandBuffer&cb ,
    uint32_t      id){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::SET_DRAW_ID;
  cmd.data.setDrawIdCommand.id = id;
}

/**
 * @brief This function inserts subCommand into command buffer.
 *
 * @param cb  command buffer
 * @param sub pointer to sub command
 */
inline void pushSubCommand(
    CommandBuffer&cb ,
    CommandBuffer*sub){
  auto&cmd=cb.commands[cb.nofCommands++];
  cmd.type = CommandType::SUB_COMMAND;
  cmd.data.subCommand.commandBuffer = sub;
}

/**
 * @brief This struct represents a mesh
 */
//! [Mesh]
struct Mesh{
  int32_t      indexBufferID  = -1               ;///< index of buffer used for indices
  size_t       indexOffset    = 0                ;///< offset into index buffer
  IndexType    indexType      = IndexType::UINT32;///< type of indices
  VertexAttrib position                          ;///< position vertex attribute
  VertexAttrib normal                            ;///< normal vertex attribute
  VertexAttrib texCoord                          ;///< tex. coord vertex attribute
  uint32_t     nofIndices     = 0                ;///< nofIndices or nofVertices (if there is no indexing)
  glm::vec4    diffuseColor   = glm::vec4(1.f)   ;///< default diffuseColor (if there is no texture)
  int          diffuseTexture = -1               ;///< diffuse texture or -1 (no texture)
  bool         doubleSided    = false            ;///< double sided material
};
//! [Mesh]

/**
 * @brief This structure represents node in tree structure of model
 */
//! [Node]
struct Node{
  glm::mat4        modelMatrix = glm::mat4(1.f);///< model transformation matrix
  int32_t          mesh        = -1;            ///< id of mesh or -1 if no mesh
  std::vector<Node>children;                    ///< list of children nodes
};
//! [Node]

/**
 * @brief This struct represent model
 */
//! [Model]
struct Model{
  std::vector<Mesh   >meshes  ;///< list of all meshes in model
  std::vector<Node   >roots   ;///< list of roots of node trees
  std::vector<Texture>textures;///< list of all textures in model
  std::vector<Buffer> buffers ;///< list of all buffers in model
};
//! [Model]
