#ifndef __GUI_PolySoupBox__
#define __GUI_PolySoupBox__

#include <GUI/GUI_PrimitiveHook.h>
#include <GR/GR_Primitive.h>

//forward declarations. Include is in cpp file
class RE_Geometry;
class RE_OGLFramebuffer;
class RE_OGLTexture;
class mySim;


namespace HDK_Sample
{
    
/// The primitive render hook which creates GUI_PolySoupBox objects.
class GUI_NeutronHook : public GUI_PrimitiveHook
{
public:
             GUI_NeutronHook();
    virtual ~GUI_NeutronHook();
    /// This is called when a new GR_Primitive is required for a polysoup.
    /// In this case, the hook is augmenting the rendering of the polysoup by
    /// drawing a bounding box and a centroid around it.
    virtual GR_Primitive  *createPrimitive(const GT_PrimitiveHandle &gt_prim,
                                           const GEO_Primitive  *geo_prim,
                                           const GR_RenderInfo  *info,
                                           const char           *cache_name,
                                           GR_PrimAcceptResult  &processed);
};
/// Primitive object that is created by GUI_PolySoupBoxHook whenever a
/// polysoup primitive is found. This object can be persistent between
/// renders, though display flag changes or navigating though SOPs can cause
/// it to be deleted and recreated later.
class GUI_Neutron : public GR_Primitive
{
public:
                         GUI_Neutron(const GR_RenderInfo *info,
                                         const char *cache_name,
                                         const GEO_Primitive *prim);
    virtual             ~GUI_Neutron();
    virtual const char *className() const { return "GUI_Neutron"; }
    /// See if the primitive can be consumed by this GR_Primitive. Only
    /// primitives from the same detail will ever be passed in. If the
    /// primitive hook specifies GUI_HOOK_COLLECT_PRIMITIVES then it is
    /// possible to have this called more than once for different GR_Primitives.
    /// A GR_Primitive that collects multiple GT or GEO primitives is
    /// responsible for keeping track of them (in a list, table, tree, etc).
    virtual GR_PrimAcceptResult acceptPrimitive(GT_PrimitiveType t,
                                                int geo_type,
                                                const GT_PrimitiveHandle &ph,
                                                const GEO_Primitive *prim);
    /// Called whenever the parent detail is changed, draw modes are changed,
    /// selection is changed, or certain volatile display options are changed
    /// (such as level of detail).
    virtual void        update(RE_Render                 *r,
                               const GT_PrimitiveHandle  &primh,
                               const GR_UpdateParms      &p);
    /// For this primitive hook, which only renders decorations, the render
    /// methods do nothing, allowing the native Houdini code to do the work.
    /// @{
    virtual void        render(RE_Render              *r,
                               GR_RenderMode           render_mode,
                               GR_RenderFlags          flags,
                               GR_DrawParms            dp);

    virtual int         renderPick(RE_Render *r,
                                   const GR_DisplayOption *opt,
                                   unsigned int pick_type,
                                   GR_PickStyle pick_style,
                                   bool has_pick_map);

    virtual void	 renderDecoration(RE_Render *r,
					  GR_Decoration decor,
					  const GR_DecorationParms &p);
    /// @}

    //virtual bool	requiresAlphaPass() const { return true; }
    //virtual bool	renderInAlphaPass(GR_AlphaPass a) { return false; }
    

private:
    
    RE_Geometry         *myGeometry;
    RE_Shader *sh;
    RE_Shader *shMain;


    RE_OGLFramebuffer *frontPosition;
    RE_OGLFramebuffer *backPosition;
    RE_Texture *frontTexture;
    RE_Texture *backTexture;
    RE_Texture *volumeTexture;

    //bind points for shader
    int fT;
    int bT;
    int vT;
    //so we can track viewport size changes
    int lastWidth = -1;
    int lastHeight = -1;

    void setupFrameBuffers(RE_Render *r, int width, int height);
    void setup3dVolume(RE_Render *r, float textureScale);
    void compileShader(RE_Render *r, mySim& simObject);

    
};
} // End HDK_Sample namespace.
#endif