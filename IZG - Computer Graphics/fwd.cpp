#include<student/fwd.hpp>

void allocate(GPUMemory&m){
  m.buffers      = new Buffer     [m.maxBuffers     ];
  m.textures     = new Texture    [m.maxTextures    ];
  m.uniforms     = new Uniform    [m.maxUniforms    ];
  m.programs     = new Program    [m.maxPrograms    ];
  m.framebuffers = new Framebuffer[m.maxFramebuffers];
  m.vertexArrays = new VertexArray[m.maxVertexArrays];
}

/**
 * @brief Constructor
 */
GPUMemory::GPUMemory(){
  maxUniforms        = 10000;
  maxVertexArrays    = 10000;
  maxTextures        = 1000 ;
  maxBuffers         = 100  ;
  maxPrograms        = 100  ;
  maxFramebuffers    = 10   ;
  defaultFramebuffer = 0    ;
  allocate(*this);
}

/**
 * @brief Copy constructor
 *
 * @param o other object
 */
GPUMemory::GPUMemory(GPUMemory const&o){
  maxUniforms        = o.maxUniforms       ;
  maxVertexArrays    = o.maxVertexArrays   ;
  maxTextures        = o.maxTextures       ;
  maxBuffers         = o.maxBuffers        ;
  maxPrograms        = o.maxPrograms       ;
  maxFramebuffers    = o.maxFramebuffers   ;
  defaultFramebuffer = o.defaultFramebuffer;
  allocate(*this);
  for(uint32_t i=0;i<maxBuffers;++i)buffers[i] = o.buffers[i];
  for(uint32_t i=0;i<maxTextures;++i)textures[i] = o.textures[i];
  for(uint32_t i=0;i<maxPrograms;++i)programs[i] = o.programs[i];
  for(uint32_t i=0;i<maxUniforms;++i)uniforms[i] = o.uniforms[i];
  for(uint32_t i=0;i<maxVertexArrays;++i)vertexArrays[i] = o.vertexArrays[i];
  for(uint32_t i=0;i<maxFramebuffers;++i)framebuffers[i] = o.framebuffers[i];
}

/**
 * @brief Assign operator
 *
 * @param o other object
 */
GPUMemory&GPUMemory::operator=(GPUMemory const&o){
  this->~GPUMemory();
  return *new(this)GPUMemory(o);
}

/**
 * @brief Destructor
 */
GPUMemory::~GPUMemory(){
  delete[] buffers     ;
  delete[] textures    ;
  delete[] uniforms    ;
  delete[] programs    ;
  delete[] framebuffers;
  delete[] vertexArrays;
}
