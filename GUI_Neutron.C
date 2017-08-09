#include "GUI_Neutron.h"
#include <DM/DM_RenderTable.h>
#include <GEO/GEO_PrimPolySoup.h>
#include <GR/GR_DisplayOption.h>
#include <GR/GR_GeoRender.h>
#include <GR/GR_OptionTable.h>
#include <GR/GR_UserOption.h>
#include <GR/GR_Utils.h>
#include <GT/GT_GEOPrimitive.h>
#include <RE/RE_Geometry.h>
#include <RE/RE_Render.h>
#include <RE/RE_OGLFramebuffer.h>
#include <RE/RE_Texture.h>

//#include <UT/UT_DSOVersion.h>
#include "utility_Neutron.h"
#include <string>
#include <cstdio>


using namespace HDK_Sample;

const char *RENDER_FLUID = "renderfluid";
const int FLUID_DRAW_GROUP = 0;

const char* vertSh = 

"#version 150 \n"
"\n"
"uniform mat4 glH_ObjectMatrix; \n"
"uniform mat4 glH_ViewMatrix; \n"
"uniform mat4 glH_ProjectMatrix; \n"
"in vec3 P; \n"
"in vec3 Cd; \n"
"out vec4 vcolor; \n"
"void main() \n"
"{ \n"
    "vcolor = vec4(Cd, 1.0); \n"
    "gl_Position = glH_ProjectMatrix * \n"
	"glH_ViewMatrix * glH_ObjectMatrix * vec4(P, 1.0); \n"
"} \n";

const char* fragSh = 
"#version 150 \n"
"in vec4 vcolor; \n"
"out vec4 color; \n"
"void main() \n"
"{ \n"
    "color = vcolor; \n"
"} \n";

const char* vertShMain = 

"#version 150 \n"
"\n"
"uniform mat4 glH_ObjectMatrix; \n"
"uniform mat4 glH_ViewMatrix; \n"
"uniform mat4 glH_ProjectMatrix; \n"
"in vec3 P; \n"
"void main() \n"
"{ \n"
    "gl_Position = glH_ProjectMatrix * \n"
	"glH_ViewMatrix * glH_ObjectMatrix * vec4(P, 1.0); \n"
"} \n";

const char* fragShMain = 
"#version 150 \n"
"out vec4 color; \n"
"uniform sampler2DRect texFront;"

"void main() \n"
"{ \n"
    "color = texture(texFront, gl_FragCoord.xy); \n"
    //"color = vec4(1.0, 0.0, 0.0, 1.0); \n"

"} \n";


//const char* vertSh = vert.c_str();
//const char* fragSh = frag.c_str();


// install the hook.
void newRenderHook(DM_RenderTable* table)
{
    // the priority only matters if multiple hooks are assigned to the same
    // primitive type. If this is the case, the hook with the highest priority
    // (largest priority value) is processed first.
    const int priority = 0;
    // register the actual hook
    table->registerGEOHook(new GUI_NeutronHook(),
        GA_PrimitiveTypeId(GA_PRIMPOLYSOUP),
        priority,
        GUI_HOOK_FLAG_AUGMENT_PRIM);
 //GUI_HOOK_FLAG_COLLECT_PRIMS
//GUI_HOOK_FLAG_AUGMENT_PRIM
    // register custom display options for the hook
    //table->installGeometryOption(RENDER_FLUID, "Render Fluid");

}
// -------------------------------------------------------------------------
// The render hook itself. It is reponsible for creating new GUI_PolySoupBox
// primitives when request by the viewport.
GUI_NeutronHook::GUI_NeutronHook()
    : GUI_PrimitiveHook("GUI_NeutronHook")
{
}
GUI_NeutronHook::~GUI_NeutronHook()
{
}
GR_Primitive*
GUI_NeutronHook::createPrimitive(const GT_PrimitiveHandle& gt_prim,
    const GEO_Primitive* geo_prim,
    const GR_RenderInfo* info,
    const char* cache_name,
    GR_PrimAcceptResult& processed)
{

        std::cout << "boocreate\n";

    GU_Detail* parent = static_cast<GU_Detail*>(geo_prim->getParent());
    //only create hook if attribute is set
    GA_RWHandleS attrib(parent, GA_ATTRIB_DETAIL, "__fluid__");
    //GA_RWHandleS attrib = parent->findFloatTuple(GA_ATTRIB_DETAIL, "__fluid__");

    if (geo_prim->getTypeId().get() == GA_PRIMPOLYSOUP && attrib.isValid()) {

    //if (geo_prim->getTypeId().get() == GA_PRIMPOLYSOUP) {
         std::cout << "boocreated!\n";

        // We're going to process this prim and prevent any more lower-priority
        // hooks from hooking on it. Alternatively, GR_PROCESSED_NON_EXCLUSIVE
        // could be passed back, in which case any hooks of lower priority would
        // also be called.
        processed = GR_PROCESSED;
        // In this case, we aren't doing anything special, like checking attribs
        // to see if this is a flagged native primitive we want to hook on.
        return new GUI_Neutron(info, cache_name, geo_prim);
    }
    return NULL;
}
// -------------------------------------------------------------------------
// The decoration rendering code for the primitive.
GUI_Neutron::GUI_Neutron(const GR_RenderInfo* info,
    const char* cache_name,
    const GEO_Primitive* prim)
    : GR_Primitive(info, cache_name, GA_PrimCompat::TypeMask(0))
{
    myGeometry = nullptr;
    sh = nullptr;
    shMain = nullptr;
    frontPosition = nullptr;
    backPosition = nullptr;
    backTexture = nullptr;
    frontTexture = nullptr;

}


GUI_Neutron::~GUI_Neutron()
{
    delete myGeometry;
    delete sh;
    delete shMain;
    delete frontPosition;
    delete backPosition;
    delete frontTexture;
    delete backTexture;
}


GR_PrimAcceptResult
GUI_Neutron::acceptPrimitive(GT_PrimitiveType t,
    int geo_type,
    const GT_PrimitiveHandle& ph,
    const GEO_Primitive* prim)
{
    GU_Detail* parent = static_cast<GU_Detail*>(prim->getParent());
    //std::cout << "boo1\n";
    //see if we have a detail attrib on there
    //GA_RWHandleS attrib = parent->findFloatTuple(GA_ATTRIB_DETAIL, "__fluid__");
    GA_RWHandleS attrib(parent, GA_ATTRIB_DETAIL, "__fluid__");
    std::cout << "startaccepted!" << std::endl;
    if (geo_type == GA_PRIMPOLYSOUP && attrib.isValid()) {
        //std::cout << "accepted" << std::endl;
        //GA_RWHandleI uniqueHandleID(parent, GA_ATTRIB_DETAIL, "uniqueHandleID");
        //mySim& simObject = myFluid::simvec[uniqueHandleID.get(0)];
        //simObject.doSomething();
        std::cout << "accepted!" << std::endl;

        return GR_PROCESSED;
    }

    return GR_NOT_PROCESSED;
}

void GUI_Neutron::setupFrameBuffers(RE_Render *r, int width, int height){

    if((!frontPosition) || (!backPosition)){
        frontPosition = new RE_OGLFramebuffer();
        backPosition = new RE_OGLFramebuffer();
                    std::cout << "created\n";
        }

        frontPosition->setResolution(width, height);
        //clean up old textures.
        delete frontTexture;
        delete backTexture;

        //create a texture rectangle
        frontTexture = frontPosition->createTexture(r, RE_GPU_FLOAT32, 4, -1, RE_COLOR_BUFFER, 0, true, 0);
        backTexture = backPosition->createTexture(r, RE_GPU_FLOAT32, 4, -1, RE_COLOR_BUFFER, 0, true, 0);
    
}


void GUI_Neutron::update(RE_Render* r,
    const GT_PrimitiveHandle& primh,
    const GR_UpdateParms& p)
{
    


    if (p.reason & (
                    GR_GEO_VISIBILITY_RESTORED | GR_GL_VIEW_CHANGED)){}
        
        UT_DimRect saved_vp = r->getViewport2DI();
        int currentWidth = saved_vp.width();
        int currentHeight = saved_vp.height();
        //std::cout << saved_vp.width() << " " << saved_vp.height() << std::endl;

        bool new_geo = false;

        if(!myGeometry)
        {
            myGeometry = new RE_Geometry(8);
            new_geo = true;
            
        }
        if( (currentWidth != lastWidth) || (currentHeight != lastHeight) ){

            setupFrameBuffers(r, currentWidth, currentWidth);
            lastWidth = currentWidth;
            lastHeight = currentHeight;
        }



        

        if(!sh){
            UT_String errors;
            sh = RE_Shader::create("My Shader");
            sh->addShader(r, RE_SHADER_VERTEX, vertSh, 
                        "optional readable name", 0);
            sh->addShader(r, RE_SHADER_FRAGMENT, fragSh, 
                        "optional readable name", 0);
            sh->linkShaders(r, &errors);
        }

        if(!shMain){
            UT_String errors;
            shMain = RE_Shader::create("My Shader");
            shMain->addShader(r, RE_SHADER_VERTEX, vertShMain, 
                        "optional readable name", 0);
            shMain->addShader(r, RE_SHADER_FRAGMENT, fragShMain, 
                        "optional readable name", 0);
            shMain->linkShaders(r, &errors);
        }

    

        //create an array of vectors to hold cube
        UT_Vector3FArray pos(8,8);
        
        pos(0) = UT_Vector3F(0.0, 0.0, 1.0);
        pos(1) = UT_Vector3F(1.0, 0.0, 1.0);
        pos(2) = UT_Vector3F(1.0, 1.0, 1.0);
        pos(3) = UT_Vector3F(0.0, 1.0, 1.0);
        
        pos(4) = UT_Vector3F(0.0, 0.0, 0.0);
        pos(5) = UT_Vector3F(1.0, 0.0, 0.0);
        pos(6) = UT_Vector3F(1.0, 1.0, 0.0);
        pos(7) = UT_Vector3F(0.0, 1.0, 0.0);

        myGeometry->createAttribute(r, "P", RE_GPU_FLOAT32, 3, pos.array()->data());



        // GR_Utils::buildInstanceObjectMatrix(r, primh, p, myGeometry,
        //                     p.instance_version);

        if(new_geo)
        {
            const uint cube_elements[] = {
                // front
                0, 1, 2,
                2, 3, 0,
                // top
                1, 5, 6,
                6, 2, 1,
                // back
                7, 6, 5,
                5, 4, 7,
                // bottom
                4, 0, 3,
                3, 7, 4,
                // left
                4, 5, 1,
                1, 0, 4,
                // right
                3, 2, 6,
                6, 7, 3,
            };

            UT_Vector3FArray col(8,8);
            col(0) = UT_Vector3F(0.0, 0.0, 1.0);
            col(1) = UT_Vector3F(1.0, 0.0, 1.0);
            col(2) = UT_Vector3F(1.0, 1.0, 1.0);
            col(3) = UT_Vector3F(0.0, 1.0, 1.0);
            
            col(4) = UT_Vector3F(0.0, 0.0, 0.0);
            col(5) = UT_Vector3F(1.0, 0.0, 0.0);
            col(6) = UT_Vector3F(1.0, 1.0, 0.0);
            col(7) = UT_Vector3F(0.0, 1.0, 0.0);
            
            myGeometry->createAttribute(r, "Cd", RE_GPU_FLOAT32, 3, col.array()->data());       

            myGeometry->connectIndexedPrims(r, FLUID_DRAW_GROUP,
                                RE_PRIM_TRIANGLES, 36, cube_elements);

            
        }

    

        

    
    
}





void GUI_Neutron::render(RE_Render* r,
    GR_RenderMode render_mode,
    GR_RenderFlags flags,
    GR_DrawParms dp)
{
    //std::cout << "rendering";

    //glDisable(GL_CULL_FACE);

    //glCullFace(GL_FRONT);

    if(render_mode == GR_RENDER_BEAUTY){
        glEnable(GL_CULL_FACE);
        //glFrontFace(GL_CCW);


        r->pushShader( sh );
        r->pushDrawFramebuffer(frontPosition);
        myGeometry->draw(r, FLUID_DRAW_GROUP);
        r->popDrawFramebuffer();
        r->popShader();
                glDisable(GL_CULL_FACE);


         r->pushShader( shMain );
        // r->pushTextureState(0);
        // r->bindTexture(frontTexture, 0);
         myGeometry->draw(r, FLUID_DRAW_GROUP);
        // r->popTextureState();
         r->popShader();



    }
    //std::cout << inc++ << "\n";
    //glDisable(GL_CULL_FACE);

    // The native Houdini primitive for polysoups will do the rendering.
}
int GUI_Neutron::renderPick(RE_Render* r,
    const GR_DisplayOption* opt,
    unsigned int pick_type,
    GR_PickStyle pick_style,
    bool has_pick_map)
{
    // The native Houdini primitive for polysoups will do the rendering.
    return 0;
}

void
GUI_Neutron::renderDecoration(RE_Render *r,
				  GR_Decoration decor,
				  const GR_DecorationParms &p)
{}