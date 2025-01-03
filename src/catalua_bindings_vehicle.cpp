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

        DOC( "Returns power info for the indexed engine, accounting for size, damage, etc. If `bool` is true, assume full HP for calculations." );
        SET_FX( part_vpower_w );

        // TODO: Getting crashes from this. Why?
        DOC( "Returns the amount of time it takes to start the engine at `int`." );
        SET_FX_T( engine_start_time, int( const int ) const );

        // TODO: Check how this interacts with the player/if it subtracts moves
        DOC( "Attempt to start a specified engine." );
        SET_FX_T( start_engine, bool( int ) );
        DOC( "Stop all of the vehicle's engines." );
        SET_FX_T( stop_engines, void() );
        DOC( "Attempt to start only the vehicle's *enabled* engines." );
        SET_FX_T( start_engines, void( bool, bool ) );

        // These seem to work, but TODO testing later.
        SET_FX( part_base );
        SET_FX( find_part );

        DOC( "Returns an (int) list of vehicle parts at a given point, relative to the vehicle itself." );
        luna::set_fx( ut, "parts_at_relative",
            []( UT_CLASS & vehi, point pt, std::optional<bool> cache ) -> std::vector<int> {
                return vehi.parts_at_relative( pt, cache.has_value() ? *cache : true );
            } );

        DOC( "Returns all the parts on a given layer/location of a vehicle, such as 'structure', 'fuel_source', or 'engine_block'." );
        SET_FX_N_T( all_parts_at_location, "parts_on_layer", std::vector<int>( const std::string & ) const );
        SET_FX_T( coord_translate, point( point ) const );
        DOC( "Turns a map Tripoint into a relative, direction-agnostic Point for use with a Vehicle. Works well with parts_at_relative()." );
        SET_FX_T( tripoint_to_mount, point( const tripoint & ) const );

        // get_parts_at
        // Requires part_status_flag enum...

        SET_FX_N_T( global_pos3, "get_pos_ms", tripoint() const );

        DOC( "Retrieve global tripoint of vehicle part, akin to Creature.get_pos_ms()." );
        luna::set_fx( ut, "get_part_pos_ms", sol::overload(
            sol::resolve<tripoint( const int & ) const>( &UT_CLASS::global_part_pos3 ),
            sol::resolve<tripoint( const vehicle_part & ) const>( &UT_CLASS::global_part_pos3 )
            ) );

        // TODO: Could this be more specific? What kind of calculation is it used for?
        DOC( "Measurement of the stress applied to engines due to running above safe speed." );
        SET_FX_T( strain, float() const );
        // TODO: How abrupt is this?
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

        luna::set_fx( ut, sol::meta_function::to_string,
            []( const UT_CLASS & st ) -> std::string {
                return string_format( "%s[%s]",
                    luna::detail::luna_traits<UT_CLASS>::name,
                    st.info().get_id().c_str() );
        } );

        luna::set( ut, "id", sol::property( &UT_CLASS::info ) );
        DOC( "The relative location of this part on its Vehicle." );
        SET_MEMB_N_RO( mount, "mount_point" );
        // XXX: DO NOT overwrite any of this!
        // TODO: May be safe to remove now; unclear
        SET_MEMB_RO( precalc );

        SET_FX_T( get_base, item & () const );
        // TODO: set_base!
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

        luna::set_fx( ut, sol::meta_function::to_string,
            []( const UT_CLASS & st ) -> std::string {
                return string_format( "%s[%s]",
                    luna::detail::luna_traits<UT_CLASS>::name,
                    st.get_id().c_str() );
        } );

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