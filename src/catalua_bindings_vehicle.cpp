#include <damage.h>
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

        // Considerations:
        // find_vehicle()
        // smash()
        // install_part, remove_part
        // get_all_parts()
        // void set_flying( bool new_flying_value );
        // collision()
        // coord_translate()
        // get_points()

        // TODO: Check how this interacts with the player/if it subtracts moves
        DOC( "Attempt to start a specified engine." );
        SET_FX_T( start_engine, bool( int ) );
        DOC( "Stop all of the vehicle's engines." );
        SET_FX_T( stop_engines, void() );
        DOC( "Attempt to start only the vehicle's *enabled* engines." );
        SET_FX_T( start_engines, void( bool, bool ) );

        DOC( "Returns an (int) list of vehicle parts at a given point, relative to the vehicle itself." );
        luna::set_fx( ut, "parts_at_relative",
            []( UT_CLASS & vehi, point pt, std::optional<bool> cache ) -> std::vector<int> {
                return vehi.parts_at_relative( pt, cache.has_value() ? *cache : true );
            } );

        DOC( "Retrieve global tripoint of vehicle part, akin Creature.get_pos_ms()." );
        luna::set_fx( ut, "global_part_pos", sol::overload(
            sol::resolve<tripoint( const int & ) const>( &UT_CLASS::global_part_pos3 ),
            sol::resolve<tripoint( const vehicle_part & ) const>( &UT_CLASS::global_part_pos3 )
            ) );

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

        DOC( "Returns the number of parts comprising this vehicle." );
        SET_FX_T( part_count, int() const );
        DOC( "Returns the VPart with the given part number." );
        SET_FX_T( part, vehicle_part &( int ) );
        DOC( "Checks if the given part number corresponds to a part on the vehicle." );
        SET_FX_N_T( valid_part, "is_valid_part", bool( int ) const );

        // Using auto to be concise; the return value is already listed in the
        // lambda definitions.
        DOC( "Returns integers corresponding to all alternators in the vehicle." );
        luna::set_fx( ut, "get_alternator_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.alternators; return rv; } );
        DOC( "Returns integers corresponding to all engines in the vehicle." );
        luna::set_fx( ut, "get_engine_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.engines; return rv; } );
        DOC( "Returns integers corresponding to all reactors in the vehicle." );
        luna::set_fx( ut, "get_reactor_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.reactors; return rv; } );
        DOC( "Returns integers corresponding to all solar panels on the vehicle." );
        luna::set_fx( ut, "get_solar_panel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.solar_panels; return rv; } );
        DOC( "Returns integers corresponding to all wind turbines on the vehicle." );
        luna::set_fx( ut, "get_wind_turbine_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.wind_turbines; return rv; } );
        DOC( "Returns integers corresponding to all water wheels on the vehicle." );
        luna::set_fx( ut, "get_water_wheel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.water_wheels; return rv; } );
        DOC( "Returns integers corresponding to all sails on the vehicle." );
        luna::set_fx( ut, "get_sail_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.sails; return rv; } );
        DOC( "Returns integers corresponding to all funnels on the vehicle." );
        luna::set_fx( ut, "get_funnel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.funnels; return rv; } );
        DOC( "Returns integers corresponding to all emitters in the vehicle." );
        luna::set_fx( ut, "get_emitter_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.emitters; return rv; } );
        DOC( "Returns integers corresponding to all UNMOUNT_ON_MOVE parts on the vehicle." );
        luna::set_fx( ut, "get_loose_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.loose_parts; return rv; } );
        DOC( "Returns integers corresponding to all wheels on the vehicle." );
        luna::set_fx( ut, "get_wheel_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.wheelcache; return rv; } );
        DOC( "Returns integers corresponding to all rotors on the vehicle." );
        luna::set_fx( ut, "get_rotor_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.rotors; return rv; } );
        DOC( "Returns integers corresponding to all (rail) wheels on the vehicle." );
        luna::set_fx( ut, "get_rail_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.rail_wheelcache; return rv; } );
        DOC( "Returns integers corresponding to all steerable parts on the vehicle." );
        luna::set_fx( ut, "get_steering_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.steering; return rv; } );
        DOC( "Returns integers corresponding to all 'specialty' parts in the vehicle." );
        luna::set_fx( ut, "get_specialty_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.speciality; return rv; } );
        DOC( "Returns integers corresponding to all buoyant parts on the vehicle." );
        luna::set_fx( ut, "get_buoyant_inds", []( UT_CLASS & vp ) -> std::vector<int> {
            auto rv = vp.floating; return rv; } );

        DOC( "Coordinates of the submap containing the vehicle." );
        SET_MEMB_RO( sm_pos );
        DOC( "Position of the vehicle inside its containing submap." );
        SET_MEMB_RO( pos );

        DOC( "Vehicle velocity, in MPH × 100." );
        SET_MEMB( velocity );

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

        luna::set( ut, "id", sol::property( &UT_CLASS::info ) );
        SET_MEMB_N_RO( mount, "mount_point" );
        SET_FX_T( get_base, item & () const );
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
        SET_MEMB_RO( item );
        SET_MEMB_N_RO( location, "layer" );
        SET_MEMB_RO( durability );
        SET_MEMB_N_RO( dmg_mod, "damage_mod" );

        SET_MEMB_N_RO( power, "net_power" );

        SET_MEMB_RO( difficulty );

        SET_MEMB_N_RO( cargo_weight_modifier, "cargo_weight_mod" );

        luna::set_fx( ut, "damage_reduction",
            []( UT_CLASS & vp ) -> std::map<damage_type, float> {
                auto rv = vp.damage_reduction.flat; return rv;
        } );

        SET_FX_T( name, std::string() const );

        luna::set_fx( ut, "get_all", []() -> std::map<vpart_id, vpart_info> {
            std::map<vpart_id, vpart_info> rv = UT_CLASS::all(); return rv;
        } );
    }
#undef UT_CLASS // #define UT_CLASS vpart_info
}

#endif // #ifdef LUA
