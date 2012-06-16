/*

	basic_latin.h

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/

#ifndef basic_latinH_4826bf0e_3e4b_4eed_8839_0651ef1416aa
#define basic_latinH_4826bf0e_3e4b_4eed_8839_0651ef1416aa

namespace basic_latin
{

	namespace control 
	{
		const unsigned char null                                     = 0x0000;
		const unsigned char start_of_heading                         = 0x0001;
		const unsigned char start_of_text                            = 0x0002;
		const unsigned char end_of_text                              = 0x0003;
		const unsigned char end_of_transmission                      = 0x0004;
		const unsigned char enquiry                                  = 0x0005;
		const unsigned char acknowledge                              = 0x0006;
		const unsigned char bell                                     = 0x0007;
		const unsigned char backspace                                = 0x0008;
		
		const unsigned char horizontal_tabulation                    = 0x0009;
		const unsigned char tab										 = 0x0009;

		const unsigned char c_newline								 = '\n';
		
		const unsigned char line_feed                                = 0x000A;
		const unsigned char vertical_tabulation                      = 0x000B;
		const unsigned char form_feed                                = 0x000C;

		const unsigned char carriage_return                          = 0x000D;

		const unsigned char c_carrige_return						 = '\r';

		const unsigned char shift_out                                = 0x000E;
		const unsigned char shift_in                                 = 0x000F;
		const unsigned char data_link_escape                         = 0x0010;
		const unsigned char device_control_one                       = 0x0011;
		const unsigned char device_control_two                       = 0x0012;
		const unsigned char device_control_three                     = 0x0013;
		const unsigned char device_control_four                      = 0x0014;
		const unsigned char negative_acknowledge                     = 0x0015;
		const unsigned char synchronous_idle                         = 0x0016;
		const unsigned char end_of_transmission_block                = 0x0017;
		const unsigned char cancel                                   = 0x0018;
		const unsigned char end_of_medium                            = 0x0019;
		const unsigned char substitute                               = 0x001A;
		const unsigned char escape                                   = 0x001B;
		const unsigned char information_separator_four               = 0x001C;
		const unsigned char information_separator_three              = 0x001D;
		const unsigned char information_separator_two                = 0x001E;
		const unsigned char information_separator_one                = 0x001F;
	}

	namespace symbol 
	{
		const unsigned char space                                    = 0x0020;
		const unsigned char exclamation_mark                         = 0x0021;
		const unsigned char quotation_mark                           = 0x0022;
		const unsigned char number_sign                              = 0x0023;
		const unsigned char dollar_sign                              = 0x0024;
		const unsigned char percent_sign                             = 0x0025;
		const unsigned char ampersand                                = 0x0026;
		const unsigned char apostrophe                               = 0x0027;
		const unsigned char left_paranthesis                         = 0x0028;
		const unsigned char right_paranthesis                        = 0x0029;
		const unsigned char asterisk                                 = 0x002A;
		const unsigned char plus_sign                                = 0x002B;
		const unsigned char comma                                    = 0x002C;
		const unsigned char hyphen_minus                             = 0x002D;
		const unsigned char full_stop                                = 0x002E;
		const unsigned char solidus                                  = 0x002F;
	}

	namespace digit 
	{
		const unsigned char zero                                     = 0x0030;
		const unsigned char one                                      = 0x0031;
		const unsigned char two                                      = 0x0032;
		const unsigned char three                                    = 0x0033;
		const unsigned char four                                     = 0x0034;
		const unsigned char five                                     = 0x0035;
		const unsigned char six                                      = 0x0036;
		const unsigned char seven                                    = 0x0037;
		const unsigned char eight                                    = 0x0038;
		const unsigned char nine                                     = 0x0039;
	}

	namespace symbol 
	{
		const unsigned char colon                                    = 0x003A;
		const unsigned char semicolon                                = 0x003B;
		const unsigned char less_than_sign                           = 0x003C;
		const unsigned char equals_sign                              = 0x003D;
		const unsigned char greater_than_sign                        = 0x003E;
		const unsigned char question_mark                            = 0x003F;
		const unsigned char commercial_at                            = 0x0040;
	}

	namespace letter 
	{
		const unsigned char capital_A                                = 0x0041;
		const unsigned char capital_B                                = 0x0042;
		const unsigned char capital_C                                = 0x0043;
		const unsigned char capital_D                                = 0x0044;
		const unsigned char capital_E                                = 0x0045;
		const unsigned char capital_F                                = 0x0046;
		const unsigned char capital_G                                = 0x0047;
		const unsigned char capital_H                                = 0x0048;
		const unsigned char capital_I                                = 0x0049;
		const unsigned char capital_J                                = 0x004A;
		const unsigned char capital_K                                = 0x004B;
		const unsigned char capital_L                                = 0x004C;
		const unsigned char capital_M                                = 0x004D;
		const unsigned char capital_N                                = 0x004E;
		const unsigned char capital_O                                = 0x004F;
		const unsigned char capital_P                                = 0x0050;
		const unsigned char capital_Q                                = 0x0051;
		const unsigned char capital_R                                = 0x0052;
		const unsigned char capital_S                                = 0x0053;
		const unsigned char capital_T                                = 0x0054;
		const unsigned char capital_U                                = 0x0055;
		const unsigned char capital_V                                = 0x0056;
		const unsigned char capital_W                                = 0x0057;
		const unsigned char capital_X                                = 0x0058;
		const unsigned char capital_Y                                = 0x0059;
		const unsigned char capital_Z                                = 0x005A;
	}

	namespace symbol 
	{
		const unsigned char left_square_bracket                      = 0x005B;
		const unsigned char reverse_solidus                          = 0x005C;
		const unsigned char right_square_bracket                     = 0x005D;
		const unsigned char circumflex_accent                        = 0x005E;
		
		const unsigned char low_line                                 = 0x005F;
		const unsigned char underscore								 = 0x005F;
		
		const unsigned char grave_accent                             = 0x0060;
	}

	namespace letter 
	{
		const unsigned char small_a                                  = 0x0061;
		const unsigned char small_b                                  = 0x0062;
		const unsigned char small_c                                  = 0x0063;
		const unsigned char small_d                                  = 0x0064;
		const unsigned char small_e                                  = 0x0065;
		const unsigned char small_f                                  = 0x0066;
		const unsigned char small_g                                  = 0x0067;
		const unsigned char small_h                                  = 0x0068;
		const unsigned char small_i                                  = 0x0069;
		const unsigned char small_j                                  = 0x006A;
		const unsigned char small_k                                  = 0x006B;
		const unsigned char small_l                                  = 0x006C;
		const unsigned char small_m                                  = 0x006D;
		const unsigned char small_n                                  = 0x006E;
		const unsigned char small_o                                  = 0x006F;
		const unsigned char small_p                                  = 0x0070;
		const unsigned char small_q                                  = 0x0071;
		const unsigned char small_r                                  = 0x0072;
		const unsigned char small_s                                  = 0x0073;
		const unsigned char small_t                                  = 0x0074;
		const unsigned char small_u                                  = 0x0075;
		const unsigned char small_v                                  = 0x0076;
		const unsigned char small_w                                  = 0x0077;
		const unsigned char small_x                                  = 0x0078;
		const unsigned char small_y                                  = 0x0079;
		const unsigned char small_z                                  = 0x007A;
	}

	namespace symbol 
	{
		const unsigned char left_curly_bracket                       = 0x007B;
		const unsigned char vertical_line                            = 0x007C;
		const unsigned char right_curly_bracket                      = 0x007D;
		const unsigned char tilde                                    = 0x007E;
	}

	namespace control 
	{
		const unsigned char delete_                                  = 0x007F;
	}

} // namespace basic_latin

namespace basic_latin
{
	inline bool is_control(unsigned char c) {
		if(c <= control::information_separator_one)
			{return true;}
		if(c == control::delete_)
			{return true;}
		return false;
	}
	inline bool is_newline(unsigned char c) {
		if(c == control::c_newline)
			return true;
		return false;
	}
	inline bool is_spacing(unsigned char c) {
		if(c == symbol::space || c == control::tab)
			return true;
		return false;
	}
	inline bool is_number(unsigned char c) {
		if(c >= digit::zero && c <= digit::nine)
			return true;
		return false;
	}
	inline bool is_basic_alpha_capital(unsigned char c) {
		if(c >= letter::capital_A && c <= letter::capital_Z)
			return true;
		return false;
	}
	inline bool is_basic_alpha_small(unsigned char c) {
		if(c >= letter::small_a && c <= letter::small_z)
			return true;
		return false;
	}
	inline bool is_basic_alpha(unsigned char c) {
		return is_basic_alpha_small(c) || is_basic_alpha_capital(c);
	}
	inline bool is_printable(unsigned char c) {
		if(is_basic_alpha(c)) return true;
		if(is_number(c)) return true;
		return false;
	}
	inline bool is_outside_basic_alpha(unsigned char c) {
		/*
			Checks to see if the byte is a utf-8 extension byte.
		*/
		return c & 128;
	}
} // namespace basic_latin

#endif