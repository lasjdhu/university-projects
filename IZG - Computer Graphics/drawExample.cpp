/*!
 * @file
 * @brief This file contains example structure of drawTriangle function
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/fwd.hpp>

///\cond HIDDEN_SYMBOLS

//![izg_enqueue_commands]

void clear(GPUMemory&mem,ClearCommand cmd){
  // ukázka čistícího příkazu

  // výběr framebufferu
  Framebuffer*fbo = mem.framebuffers+mem.activatedFramebuffer;

  // máme čistit barvu framebufferu?
  if(cmd.clearColor){
    // obsahuje framebuffer barevný buffer?
    if(fbo->color.data){
    //...
    }
  }
}

void izg_enqueue(GPUMemory&mem,CommandBuffer const&cb){
  for(uint32_t i=0;i<cb.nofCommands;++i){
    CommandType type = cb.commands[i].type;
    CommandData data = cb.commands[i].data;
    if(type == CommandType::CLEAR)
      clear(mem,data.clearCommand);
  }
}
//![izg_enqueue_commands]

//![izg_enqueue_before_vs]
void draw(GPUMemory&mem,DrawCommand cmd){
  // výběr programu
  Program prg = mem.programs[mem.activatedProgram];

  for(every vertex v < cmd.nofVertices){
    InVertex inVertex;
    OutVertex outVertex;
    ShaderInterface si;
    si.gl_DrawID = mem.gl_DrawID;
    // spuštění vertex shaderu
    prg.vertexShader(outVertex,inVertex,si);
  }
}

void izg_enqueue(GPUMemory&mem,CommandBuffer const&cb){
  // smyčka přes příkazy
  for(... commands ...){
    // vykreslovací příkaz
    if (commandType == CommandType::DRAW ){
      // kresli
      draw(mem, drawCommand);
      // počítadlo kreslících příkazů
      mem.gl_DrawID++;
    }
    if (commandType == CommandType::SET_DRAW_ID){
      mem.gl_DrawID = ...
    }
  }
}

//![izg_enqueue_before_vs]



//![izg_enqueue_after_vs]
void runVertexAssembly(){
  computeVertexID();
  readAttributes();
}

void runPrimitiveAssembly(primitive,vertexArray,t,program){
  for(every vertex v in triangle){
    InVertex inVertex;
    runVertexAssembly(inVertex,vertexArray,t+v);
    ShaderInterface si;
    program.vertexShader(primitive.vertex,inVertex,si);
  }
}

void rasterizeTriangle(framebuffer,primitive,prrogram){
  for(pixels in frame){
    if(pixels in primitive){
      InFragment inFragment;
      createFragment(inFragment,primitive,barycentrics,pixelCoord,program);
      OutFragment outFragment;
      ShaderInterface si;
      program.fragmentShader(outFragment,inFragment,si);
    }
  }
}

void draw(GPUMemory&mem,DrawCommand const&cmd){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,vertexArray,t,program)

    runPerspectiveDivision(primitive)
    runViewportTransformation(primitive,width,height)
    rasterizeTriangle(framebuffer,primitive,program);
  }
}

void izg_enqueue(GPUMemory&mem,CommandBuffer const&cb){

}
//![izg_enqueue_after_vs]




//![izg_enqueue_pfo]
void runVertexAssembly(){
  computeVertexID()
  readVertexAttributes();
}

void runPrimitiveAssembly(primitive,VertexArray vao,t,vertexShader,shaderInterface){
  for(every vertex v in triangle){
    InVertex inVertex;
    runVertexAssembly(inVertex,vao,t+v);
    vertexShader(primitive.vertex,inVertex,shaderInterface);
  }
}

void rasterizeTriangle(framebuffer,primitive,fragmentShader){
  for(pixels in frame){
    if(pixels in primitive){
      InFragment inFragment;
      createFragment(inFragment,primitive,barycentrics,pixelCoord,prg);
      OutFragment outFragment;
      ShaderInterface si;
      si.uniforms = ...;
      si.textures = ...;
      
      fragmentShader(outFragment,inFragment,si);
      clampColor(outFragment,0,1);
      perFragmentOperations(framebuffer,outFragment,inFragment.gl_FragCoord.z)
    }
  }

}

void draw(GPUMemory&mem,DrawCommand const&cmd){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,vao,t,vertexShader,shaderInterface)

    runPerspectiveDivision(primitive)
    runViewportTransformation(primitive,width,height)
    rasterizeTriangle(framebuffer,primitive,fragmentShader);
  }
}

void izg_enqueue(...){
}

//![izg_enqueue_pfo]



//![izg_enqueue]
void draw(mem,drawCommand){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,vertexArray,t,vertexShader)

    ClippedPrimitive clipped;
    performeClipping(clipped,primitive);

    for(all clipped triangle c in clipped){
      runPerspectiveDivision(c)
      runViewportTransformation(c,width,height)
      rasterizeTriangle(framebuffer,c,fragmentShader);
    }
  }
}

void izg_enqueue(mem,commandBuffer){
  for(every command in commandBuffer){
    if(isDrawCommand )draw (mem,drawCommand)
  }
}

//![izg_enqueue]


//![prepareModel]
void prepareNode(GPUMemory&mem,CommandBuffer&cb,Node const&node,Model const&model,glm::mat4 const&prubeznaMatice,...){
  if(node.mesh>=0){
    Mesh mesh = model.meshes[node.mesh];

    drawCounter; // pocitadlo kreslicich prikazu

    // vytvoření vertex array
    VertexArray vao;
    vao.indexBufferID = mesh.indexBufferID;
    vao.indexOffset = ...;
    vao.indexType = ...;
    vao.vertexAttrib[0] = ...; // pozice
    vao.vertexAttrib[1] = ...; // normala
    vao.vertexAttrib[2] = ...; // texturovaci souradnice

    // vlozeni vao na spravne misto v pameti (aby jej bylo mozne najit 
    mem.vertexArrays[drawCounter] = vao;

    BindVertexArrayCommand bindVaoCmd;
    bindVaoCmd.id = drawCounter;

    DrawCommand drawCmd;
    drawCmd.backfaceCulling = ...; // pokud je double sided tak by se nemelo orezavat
    drawCmd.nofVertices = ...; // pocet vertexu

    // vlozeni bindVaoCmd a drawCmd do command buffer cb
    cb.commands[...    ] = bindVaoCmd;
    cb.commands[... +1 ] = drawCmd   ;
    
    //zapis uniformnich dat do pameti
    ZKOBINUJ(prubeznaMatice,node.modelMatrix);
    vypocitej inverzni transponovanou matici pro normaly...

    mem.uniforms[10+drawCounter*5+0].m4 = modelMatrix
    mem.uniforms[10+drawCounter*5+1].m4 = inverzniTransponovana Modelova
    mem.uniforms[10+drawCounter*5+2].v4 = difuzni barva
    mem.uniforms[10+drawCounter*5+3].i1 = id textury nebo -1 pokud neni
    mem.uniforms[10+drawCounter*5+4].v1 = double sided
    writeToMemory(mem);
  }

  for(size_t i=0;i<node.children.size();++i)
    prepareNode(mem,node.children[i],model,...); rekurze
}

void prepareModel(GPUMemory&mem,CommandBuffer&cb,Model const&model){
  mem.buffers  = ...;
  mem.textures = ...;

  glm::mat4 jednotkovaMatrice = glm::mat4(1.f);
  for(size_t i=0;i<model.roots.size();++i)
    prepareNode(mem,cb,model.roots[i],jednotkovaMatrice,...);
}
//![prepareModel]

///\endcond
