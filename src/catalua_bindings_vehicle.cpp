#ifdef LUA
#include <type_id.h>
#include "catalua_bindings.h"

#include "catalua.h"
// Thx Almantuxas
#include "catalua_bindings_utils.h"
#include "catalua_impl.h"
#include "catalua_log.h"
#include "catalua_luna.h"
#include "catalua_luna_doc.h"

#include "creature.h"
#include "itype.h"
#include "vehicle.h"
#include "vehicle_part.h"
#include "veh_type.h"

// IN WAITING: TODO
void cata::detail::reg_vehicle_family( sol::state &lua ) {
    reg_vehicle( lua );
    reg_vehicle_part( lua );
    reg_vpart_info( lua );
}

void cata::detail::reg_vehicle( sol::state &lua )
{
#define UT_CLASS vehicle
    {
        /* NOTE: Well!
         * This...is going to take a while.
         */
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::no_constructor
        );

        SET_MEMB( velocity );

        // Considerations:
        // find_vehicle()
        // smash()
        // install_part, remove_part
        // get_all_parts()
        // void set_flying( bool new_flying_value );
        // collision()

        // TODO: Check how this interacts with the player/if it subtracts moves
        DOC( "Attempt to start a specified engine." );
        SET_FX_T( start_engine, bool( int ) );
        DOC( "Stop all of the vehicle's engines." );
        SET_FX_T( stop_engines, void() );
        DOC( "Attempt to start only the vehicle's *enabled* engines." );
        SET_FX_T( start_engines, void( bool, bool ) );

        // TODO: Could this be more specific? What kind of calculation is it used for?
        DOC( "Measurement of the stress applied to engines due to running above safe speed." );
        SET_FX_T( strain, float() const );

        // How abrupt is this?
        DOC( "Reduces velocity to zero." );
        SET_FX_T( stop, void( bool ) );

        // This...is kind of a mess—the integer seems to be an index to a
        // specific group of parts (in this case, engines).
        DOC( "Toggle the engine at `int` on or off." );
        SET_FX_T( toggle_specific_engine, void( int, bool ) );

        SET_FX_T( part, vehicle_part &( int ) );

        luna::set_fx( ut, "get_alternator_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.alternators; return rv; } );
        luna::set_fx( ut, "get_engine_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.engines; return rv; } );
        luna::set_fx( ut, "get_reactor_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.reactors; return rv; } );
        luna::set_fx( ut, "get_solar_panel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.solar_panels; return rv; } );
        luna::set_fx( ut, "get_wind_turbine_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.wind_turbines; return rv; } );
        luna::set_fx( ut, "get_water_wheel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.water_wheels; return rv; } );
        luna::set_fx( ut, "get_sail_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.sails; return rv; } );
        luna::set_fx( ut, "get_funnel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.funnels; return rv; } );
        luna::set_fx( ut, "get_emitter_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.emitters; return rv; } );
        luna::set_fx( ut, "get_loose_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.loose_parts; return rv; } );
        luna::set_fx( ut, "get_wheel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.wheelcache; return rv; } );
        luna::set_fx( ut, "get_rotor_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.rotors; return rv; } );
        luna::set_fx( ut, "get_rail_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.rail_wheelcache; return rv; } );
        luna::set_fx( ut, "get_steering_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.steering; return rv; } );
        luna::set_fx( ut, "get_specialty_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.speciality; return rv; } );
        luna::set_fx( ut, "get_buoyant_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.floating; return rv; } );

        // Check gates.cpp:274 and vpart_position.h for uses of
        // optional_vpart_position
        
    }
#undef UT_CLASS // #define UT_CLASS vehicle
}
void cata::detail::reg_vehicle_part( sol::state &lua )
{
#define UT_CLASS vehicle_part
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::no_constructor
        );
    }
#undef UT_CLASS // #define UT_CLASS vehicle_part
}

void cata::detail::reg_vpart_info( sol::state &lua )
{
#define UT_CLASS vpart_info
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::no_constructor
        );

        SET_MEMB_RO( id );
        //SET_MEMB_RO( item );
        luna::set( ut, "item", sol::readonly( &UT_CLASS::item ) );
        SET_MEMB_N_RO( location, "layer" );
        SET_MEMB_RO( durability );
        SET_MEMB_N_RO( dmg_mod, "damage_mod" );

        SET_MEMB_RO( difficulty );

        SET_MEMB_N_RO( cargo_weight_modifier, "cargo_weight_mod" );

        SET_FX_T( name, std::string() const );

        luna::set_fx( ut, "get_all", []() -> std::map<vpart_id, vpart_info> {
            std::map<vpart_id, vpart_info> rv = UT_CLASS::all(); return rv;
        } );
        //luna::set_fx( ut, "get_all",
        //        sol::resolve< static const std::map<vpart_id, vpart_info> >( &UT_CLASS::all )
        //        );
    }
#undef UT_CLASS // #define UT_CLASS vpart_info
}

#endif // #ifdef LUA
