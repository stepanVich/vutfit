/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3) ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret* definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_gethandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()


    
 
  UniformLocation const viewMatrix       = getUniformLocation(gpu, "viewMatrix");
  Uniforms        const handle           = gpu_getUniformsHandle(gpu);  
  Mat4            const*const view       = shader_interpretUniformAsMat4(handle, viewMatrix);
  UniformLocation const projMatrix       = getUniformLocation(gpu, "projectionMatrix");
  Mat4            const*const projection = shader_interpretUniformAsMat4(handle, projMatrix);
  Vec3            const*const position   = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
  

  Mat4 multiply;
  Vec4 position_result;
  Vec3            *const positionOut = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
  Vec3            const*const   norm = vs_interpretInputVertexAttributeAsVec3( gpu,  input, 1);
  Vec3            *const     normOut = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);

  multiply_Mat4_Mat4(&multiply, projection, view);
  copy_Vec3Float_To_Vec4(&position_result, position, 1.f);
  multiply_Mat4_Vec4(&output->gl_Position, &multiply, &position_result);
  init_Vec3(positionOut, position->data[0], position->data[1], position->data[2]);
  init_Vec3(normOut, norm->data[0], norm->data[1], norm->data[2]);

  (void)output;
  (void)input;
  (void)gpu;

}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ) {

  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br> 
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnných.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte na čistou zelenou.
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()


  Vec3 const* vnitrniPozice  = fs_interpretInputAttributeAsVec3(gpu, input, 0);
  Vec3 const* normovanaPozice = fs_interpretInputAttributeAsVec3(gpu, input, 1);
 
  UniformLocation const poziceKamery = getUniformLocation(gpu, "cameraPosition");
  UniformLocation const poziceSvetla = getUniformLocation(gpu, "lightPosition");
  Uniforms const handle              = gpu_getUniformsHandle(gpu);
  Vec3 const*const svetlo            = shader_interpretUniformAsVec3(handle, poziceSvetla);
  Vec3 const*const kamera            = shader_interpretUniformAsVec3(handle, poziceKamery);

  // Deklarace promennych
  Vec3 normala;
  Vec3 barvaTelesa;  
  Vec3 substituce;
  Vec3 substituce2;  
  Vec3 barvaSvetla;
  Vec3 rozptyl;
  Vec3 spec2;
  Vec3 zobrazeni;
  Vec3 reflektovani;
  Vec3 svetelnaSlozka;  
  Vec3 A;
  Vec3 vysledek;


  normalize_Vec3(&normala, normovanaPozice);
  sub_Vec3(&substituce, svetlo, vnitrniPozice);
  normalize_Vec3(&svetelnaSlozka, &substituce); 
  float diff = dot_Vec3(&normala, &svetelnaSlozka);
  if (diff < 0)
    diff = 0;



  init_Vec3(&barvaTelesa, 0 ,1 ,0);
  multiply_Vec3_Float(&rozptyl, &barvaTelesa, diff);
  init_Vec3(&barvaSvetla, 1 ,1 ,1);
  sub_Vec3(&substituce2, kamera, vnitrniPozice);
  normalize_Vec3(&zobrazeni, &substituce2); 
  multiply_Vec3_Float(&reflektovani ,&svetelnaSlozka ,-1.f);
  reflect(&A, &reflektovani, &normala);

  float spec =  dot_Vec3(&zobrazeni, &A);
  if (spec < 0)
    spec = 0;
  
  float spec3;
  float shine = 40;
  spec3 = powf(spec, shine);

  multiply_Vec3_Float(&spec2, &barvaSvetla, spec3);
  add_Vec3(&vysledek, &rozptyl, &spec2);
  copy_Vec3Float_To_Vec4(&output->color, &vysledek, 1.f);

  (void)output;
  (void)input;
  (void)gpu;

}

/// @}
