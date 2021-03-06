

typedef enum
{
LIBERTY_GROUPENUM_lut					= 1,
LIBERTY_GROUPENUM_wire_load_table					= 2,
LIBERTY_GROUPENUM_wire_load_selection					= 3,
LIBERTY_GROUPENUM_interconnect_delay					= 4,
LIBERTY_GROUPENUM_wire_load					= 5,
LIBERTY_GROUPENUM_user_parameters					= 6,
LIBERTY_GROUPENUM_timing_range					= 7,
LIBERTY_GROUPENUM_scaling_factors					= 8,
LIBERTY_GROUPENUM_scaled_cell					= 9,
LIBERTY_GROUPENUM_rise_transition_degradation					= 10,
LIBERTY_GROUPENUM_rise_net_delay					= 11,
LIBERTY_GROUPENUM_propagation_lut_template					= 12,
LIBERTY_GROUPENUM_power_supply					= 13,
LIBERTY_GROUPENUM_power_poly_template					= 14,
LIBERTY_GROUPENUM_power_lut_template					= 15,
LIBERTY_GROUPENUM_poly_template					= 16,
LIBERTY_GROUPENUM_output_voltage					= 17,
LIBERTY_GROUPENUM_ccs_lu_table_template					= 18,
LIBERTY_GROUPENUM_pg_current_template					= 19,
LIBERTY_GROUPENUM_output_current_template					= 20,
LIBERTY_GROUPENUM_normalized_driver_waveform					= 21,
LIBERTY_GROUPENUM_operating_conditions					= 22,
LIBERTY_GROUPENUM_noise_lut_template					= 23,
LIBERTY_GROUPENUM_model					= 24,
LIBERTY_GROUPENUM_maxtrans_lut_template					= 25,
LIBERTY_GROUPENUM_maxcap_lut_template					= 26,
LIBERTY_GROUPENUM_lu_table_template					= 27,
LIBERTY_GROUPENUM_compact_lut_template					= 28,
LIBERTY_GROUPENUM_sensitization					= 29,
LIBERTY_GROUPENUM_base_curves					= 30,
LIBERTY_GROUPENUM_ccs_timing_base_curve					= 31,
LIBERTY_GROUPENUM_ccs_timing_base_curve_template					= 32,
LIBERTY_GROUPENUM_iv_lut_template					= 33,
LIBERTY_GROUPENUM_input_voltage					= 34,
LIBERTY_GROUPENUM_fall_transition_degradation					= 35,
LIBERTY_GROUPENUM_fall_net_delay					= 36,
LIBERTY_GROUPENUM_em_lut_template					= 37,
LIBERTY_GROUPENUM_type					= 38,
LIBERTY_GROUPENUM_test_cell					= 39,
LIBERTY_GROUPENUM_statetable					= 40,
LIBERTY_GROUPENUM_state					= 41,
LIBERTY_GROUPENUM_seq_bank					= 42,
LIBERTY_GROUPENUM_seq					= 43,
LIBERTY_GROUPENUM_routing_track					= 44,
LIBERTY_GROUPENUM_mode_value					= 45,
LIBERTY_GROUPENUM_mode_definition					= 46,
LIBERTY_GROUPENUM_memory					= 47,
LIBERTY_GROUPENUM_gate_leakage					= 48,
LIBERTY_GROUPENUM_leakage_current					= 49,
LIBERTY_GROUPENUM_intrinsic_capacitance					= 50,
LIBERTY_GROUPENUM_intrinsic_resistance					= 51,
LIBERTY_GROUPENUM_intrinsic_parasitic					= 52,
LIBERTY_GROUPENUM_pg_current					= 53,
LIBERTY_GROUPENUM_switching_group					= 54,
LIBERTY_GROUPENUM_dynamic_current					= 55,
LIBERTY_GROUPENUM_leakage_power					= 56,
LIBERTY_GROUPENUM_latch_bank					= 57,
LIBERTY_GROUPENUM_latch					= 58,
LIBERTY_GROUPENUM_generated_clock					= 59,
LIBERTY_GROUPENUM_ff_bank					= 60,
LIBERTY_GROUPENUM_ff					= 61,
LIBERTY_GROUPENUM_bus					= 62,
LIBERTY_GROUPENUM_pin_based_variation					= 63,
LIBERTY_GROUPENUM_tlatch					= 64,
LIBERTY_GROUPENUM_va_fall_constraint					= 65,
LIBERTY_GROUPENUM_va_rise_constraint					= 66,
LIBERTY_GROUPENUM_va_receiver_capacitance2_rise					= 67,
LIBERTY_GROUPENUM_va_receiver_capacitance2_fall					= 68,
LIBERTY_GROUPENUM_va_receiver_capacitance1_rise					= 69,
LIBERTY_GROUPENUM_va_receiver_capacitance1_fall					= 70,
LIBERTY_GROUPENUM_va_compact_ccs_fall					= 71,
LIBERTY_GROUPENUM_va_compact_ccs_rise					= 72,
LIBERTY_GROUPENUM_timing_based_variation					= 73,
LIBERTY_GROUPENUM_steady_state_current_tristate					= 74,
LIBERTY_GROUPENUM_steady_state_current_low					= 75,
LIBERTY_GROUPENUM_steady_state_current_high					= 76,
LIBERTY_GROUPENUM_rise_transition					= 77,
LIBERTY_GROUPENUM_rise_propagation					= 78,
LIBERTY_GROUPENUM_rise_constraint					= 79,
LIBERTY_GROUPENUM_retaining_rise					= 80,
LIBERTY_GROUPENUM_retaining_fall					= 81,
LIBERTY_GROUPENUM_retain_rise_slew					= 82,
LIBERTY_GROUPENUM_retain_fall_slew					= 83,
LIBERTY_GROUPENUM_propagated_noise_width_low					= 84,
LIBERTY_GROUPENUM_propagated_noise_width_high					= 85,
LIBERTY_GROUPENUM_propagated_noise_width_below_low					= 86,
LIBERTY_GROUPENUM_propagated_noise_width_above_high					= 87,
LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_low					= 88,
LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_high					= 89,
LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_below_low					= 90,
LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_above_high					= 91,
LIBERTY_GROUPENUM_propagated_noise_height_low					= 92,
LIBERTY_GROUPENUM_propagated_noise_height_high					= 93,
LIBERTY_GROUPENUM_propagated_noise_height_below_low					= 94,
LIBERTY_GROUPENUM_propagated_noise_height_above_high					= 95,
LIBERTY_GROUPENUM_output_current_rise					= 96,
LIBERTY_GROUPENUM_right_id					= 97,
LIBERTY_GROUPENUM_left_id					= 98,
LIBERTY_GROUPENUM_peak_time					= 99,
LIBERTY_GROUPENUM_peak_voltage					= 100,
LIBERTY_GROUPENUM_peak_current					= 101,
LIBERTY_GROUPENUM_init_current					= 102,
LIBERTY_GROUPENUM_compressed_ccs_timing_rise					= 103,
LIBERTY_GROUPENUM_compact_ccs_fall					= 104,
LIBERTY_GROUPENUM_compact_ccs_rise					= 105,
LIBERTY_GROUPENUM_output_current_fall					= 106,
LIBERTY_GROUPENUM_noise_immunity_low					= 107,
LIBERTY_GROUPENUM_noise_immunity_high					= 108,
LIBERTY_GROUPENUM_noise_immunity_below_low					= 109,
LIBERTY_GROUPENUM_noise_immunity_above_high					= 110,
LIBERTY_GROUPENUM_fall_transition					= 111,
LIBERTY_GROUPENUM_fall_propagation					= 112,
LIBERTY_GROUPENUM_fall_constraint					= 113,
LIBERTY_GROUPENUM_cell_rise_to_pct					= 114,
LIBERTY_GROUPENUM_cell_rise					= 115,
LIBERTY_GROUPENUM_cell_fall_to_pct					= 116,
LIBERTY_GROUPENUM_cell_fall					= 117,
LIBERTY_GROUPENUM_cell_degradation					= 118,
LIBERTY_GROUPENUM_timing					= 119,
LIBERTY_GROUPENUM_receiver_capacitance2_rise					= 120,
LIBERTY_GROUPENUM_receiver_capacitance2_fall					= 121,
LIBERTY_GROUPENUM_receiver_capacitance1_rise					= 122,
LIBERTY_GROUPENUM_receiver_capacitance1_fall					= 123,
LIBERTY_GROUPENUM_receiver_capacitance					= 124,
LIBERTY_GROUPENUM_rise_capacitance_range					= 125,
LIBERTY_GROUPENUM_rise_capacitance					= 126,
LIBERTY_GROUPENUM_upper					= 127,
LIBERTY_GROUPENUM_lower					= 128,
LIBERTY_GROUPENUM_fall_capacitance_range					= 129,
LIBERTY_GROUPENUM_fall_capacitance					= 130,
LIBERTY_GROUPENUM_capacitance					= 131,
LIBERTY_GROUPENUM_pin_capacitance					= 132,
LIBERTY_GROUPENUM_ccsn_last_stage					= 133,
LIBERTY_GROUPENUM_propagated_noise_high					= 134,
LIBERTY_GROUPENUM_propagated_noise_low					= 135,
LIBERTY_GROUPENUM_output_voltage_fall					= 136,
LIBERTY_GROUPENUM_vector					= 137,
LIBERTY_GROUPENUM_output_voltage_rise					= 138,
LIBERTY_GROUPENUM_ccsn_first_stage					= 139,
LIBERTY_GROUPENUM_output_signal_swing					= 140,
LIBERTY_GROUPENUM_input_signal_swing					= 141,
LIBERTY_GROUPENUM_pin					= 142,
LIBERTY_GROUPENUM_minimum_period					= 143,
LIBERTY_GROUPENUM_min_pulse_width					= 144,
LIBERTY_GROUPENUM_memory_write					= 145,
LIBERTY_GROUPENUM_memory_read					= 146,
LIBERTY_GROUPENUM_max_trans					= 147,
LIBERTY_GROUPENUM_max_cap					= 148,
LIBERTY_GROUPENUM_rise_power					= 149,
LIBERTY_GROUPENUM_power					= 150,
LIBERTY_GROUPENUM_fall_power					= 151,
LIBERTY_GROUPENUM_internal_power					= 152,
LIBERTY_GROUPENUM_hyperbolic_noise_low					= 153,
LIBERTY_GROUPENUM_hyperbolic_noise_high					= 154,
LIBERTY_GROUPENUM_hyperbolic_noise_below_low					= 155,
LIBERTY_GROUPENUM_hyperbolic_noise_above_high					= 156,
LIBERTY_GROUPENUM_domain					= 157,
LIBERTY_GROUPENUM_em_max_toggle_rate					= 158,
LIBERTY_GROUPENUM_electromigration					= 159,
LIBERTY_GROUPENUM_bundle					= 160,
LIBERTY_GROUPENUM_dc_current					= 161,
LIBERTY_GROUPENUM_pg_pin					= 162,
LIBERTY_GROUPENUM_cell					= 163,
LIBERTY_GROUPENUM_library					= 164,
} group_enum;

