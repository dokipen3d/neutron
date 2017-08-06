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
    GU_Detail* parent = static_cast<GU_Detail*>(geo_prim->getParent());
    //only create hook if attribute is set
    GA_RWHandleS attrib(parent, GA_ATTRIB_DETAIL, "__fluid__");

    if (geo_prim->getTypeId().get() == GA_PRIMPOLYSOUP && attrib.isValid()) {

    //if (geo_prim->getTypeId().get() == GA_PRIMPOLYSOUP) {

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
    myGeometry = NULL;

    

}


GUI_Neutron::~GUI_Neutron()
{
    delete myGeometry;
}


GR_PrimAcceptResult
GUI_Neutron::acceptPrimitive(GT_PrimitiveType t,
    int geo_type,
    const GT_PrimitiveHandle& ph,
    const GEO_Primitive* prim)
{
    GU_Detail* parent = static_cast<GU_Detail*>(prim->getParent());

    //see if we have a detail attrib on there
    GA_RWHandleS attrib(parent, GA_ATTRIB_DETAIL, "__fluid__");

    if (geo_type == GA_PRIMPOLYSOUP && attrib.isValid()) {
        //std::cout << "accepted" << std::endl;
        GA_RWHandleI uniqueHandleID(parent, GA_ATTRIB_DETAIL, "uniqueHandleID");
        mySim& simObject = myFluid::simvec[uniqueHandleID.get(0)];
        simObject.doSomething();
        return GR_PROCESSED;
    }

    return GR_NOT_PROCESSED;
}
void GUI_Neutron::update(RE_Render* r,
    const GT_PrimitiveHandle& primh,
    const GR_UpdateParms& p)
{
    


    if (p.reason & (GR_GEO_CHANGED | GR_GEO_TOPOLOGY_CHANGED | GR_INSTANCE_PARMS_CHANGED)) {}
        
        UT_DimRect saved_vp = r->getViewport2DI();
            std::cout << saved_vp.width() << " " << saved_vp.height() << std::endl;

        bool new_geo = false;

        if(!myGeometry)
        {
            myGeometry = new RE_Geometry(8);
            myGeometry->setNumPoints( 8);
            new_geo = true;
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

        

        // GR_Utils::buildInstanceObjectMatrix(r, primh, p, myGeometry,
        //                     p.instance_version);

        //if(new_geo)
        //{
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
            
            
            
        //}

        myGeometry->createAttribute(r, "P", RE_GPU_FLOAT32, 3, pos.array()->data(), RE_ARRAY_POINT, 8);
        myGeometry->createAttribute(r, "Cd", RE_GPU_FLOAT32, 3, col.array()->data(), RE_ARRAY_POINT, 8);       

        myGeometry->connectIndexedPrims(r, FLUID_DRAW_GROUP,
                                RE_PRIM_TRIANGLES, 36, cube_elements);

        std::cout << "connected\n";
    
    
}



void
GUI_Neutron::renderFluid(RE_Render *r,
			    const GR_DisplayOption *opts)
{

    //r->pushShader( GR_Utils::getColorShader(r) );
    r->pushShader(sh);
    //std::cout << "draw\n";
    //myGeometry->draw(r, FLUID_DRAW_GROUP);
    
    r->popShader();


}

void GUI_Neutron::render(RE_Render* r,
    GR_RenderMode render_mode,
    GR_RenderFlags flags,
    GR_DrawParms dp)
{

    if(!sh){
        std::cout << "got here beforecreate\n";

        sh = RE_Shader::create("My Shader");
        std::cout << "got here aftercreate\n";

        UT_String errors;
        // sh->addShader(r, RE_SHADER_VERTEX, vertSh, 
        //             "optional readable name", 150, // glsl version
        //             &errors);
        // sh->addShader(r, RE_SHADER_FRAGMENT, fragSh, 
        //             "optional readable name", 150,
        //             &errors);
        std::cout << "got here before add vert\n";
        sh->addShader(r, RE_SHADER_VERTEX, vertSh, 
                    "optional readable name", 0);
        std::cout << "got here after add vert\n";

        sh->addShader(r, RE_SHADER_FRAGMENT, fragSh, 
                    "optional readable name", 0);
        std::cout << "got here after add frag\n";

        sh->linkShaders(r, &errors);
        std::cout << "linked\n";


        //std::cout << errors.c_str() << "\n";
        std::cout << "linked2\n";

    }
    r->pushShader( sh );
    //std::cout << "draw\n";
    myGeometry->draw(r, FLUID_DRAW_GROUP);
    
    r->popShader();
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