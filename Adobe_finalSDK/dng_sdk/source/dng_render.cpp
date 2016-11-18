/*****************************************************************************/
// Copyright 2006-2007 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_4/dng_sdk/source/dng_render.cpp#1 $ */ 
/* $DateTime: 2012/05/30 13:28:51 $ */
/* $Change: 832332 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#include "dng_render.h"

#include "dng_1d_table.h"
#include "dng_bottlenecks.h"
#include "dng_camera_profile.h"
#include "dng_color_space.h"
#include "dng_color_spec.h"
#include "dng_filter_task.h"
#include "dng_host.h"
#include "dng_image.h"
#include "dng_negative.h"
#include "dng_resample.h"
#include "dng_utils.h"

//can hakki added for printf print 
#include "dng_globals.h" 
#include <iostream>
#include <fstream>	
#include <istream>	
#include <string>	

#include <direct.h>// create directory for stageN images 
#include <string>     // std::string, std::to_string
/*****************************************************************************/

dng_function_exposure_ramp::dng_function_exposure_ramp (real64 white,
														real64 black,
														real64 minBlack)
									
	:	fSlope (1.0 / (white - black))
	,	fBlack (black)
	
	,	fRadius (0.0)//can commented out changed fRadius (0.0) to fRadius (0.5) than back
	,	fQScale (0.0)
	
	{
	
	const real64 kMaxCurveX = 0.5;			// Fraction of minBlack.
	
	const real64 kMaxCurveY = 1.0 / 16.0;	// Fraction of white.
	
	fRadius = Min_real64 (kMaxCurveX * minBlack,
						  kMaxCurveY / fSlope);
	if (fRadius > 0.0)
		fQScale= fSlope / (4.0 * fRadius);
	else
		fQScale = 0.0;

	#if qDNGValidate
				
	if (gVerbose)
		{
											
			printf ("    fRadius = %.4f\n",
					fRadius);						
			printf ("    fQScale = %.4f\n",
					fQScale);													
			printf ("    fSlope = %.4f\n",
					fSlope);	
			printf ("    fBlack = %.4f\n",
					fBlack);	
			printf ("    black = %.4f\n",
					black);	
			printf ("    white = %.4f\n",
					white);	
			printf ("    minBlack = %.4f\n",
					minBlack);	
	}
					
	#endif
	}
			
/*****************************************************************************/

real64 dng_function_exposure_ramp::Evaluate (real64 x) const
	{
	
	if (x <= fBlack - fRadius)
		return 0.0;
		
	if (x >= fBlack + fRadius)
		return Min_real64 ((x - fBlack) * fSlope, 1.0);
		
	real64 y = x - (fBlack - fRadius);
	
	return fQScale * y * y;
	
	}
			
/*****************************************************************************/

dng_function_exposure_tone::dng_function_exposure_tone (real64 exposure)
		
	:	fIsNOP (exposure >= 0.0)
	
	,	fSlope (0.0)
	
	,	a (0.0)
	,	b (0.0)
	,	c (0.0)
	
	{
	
	if (!fIsNOP)
		{
		
		// Find slope to use for the all except the highest two f-stops.
		
		fSlope = pow (2.0, exposure);
		
		// Find quadradic parameters that match this darking at the crossover
		// point, yet still map pure white to pure white.
	
		a = 16.0 / 9.0 * (1.0 - fSlope);
	
		b = fSlope - 0.5 * a;
	
		c = 1.0 - a - b;

		}
	
	}
	
/*****************************************************************************/

real64 dng_function_exposure_tone::Evaluate (real64 x) const
	{
	
	if (!fIsNOP)
		{
		
		if (x <= 0.25)
			x = x * fSlope;
			
		else
			x = (a * x + b) * x + c;

		}
		
	return x;
	
	}
	
/*****************************************************************************/

real64 dng_tone_curve_acr3_default::Evaluate (real64 x) const
	{
	std::fstream myfile(".\\Tone2.txt", std::ios_base::in);

real32 value1;
real32 value2;
real32 value3;
real32 value4;
real32 value5;
real32 value6;
real32 value7;
real32 value8;
real32 value9;
real32 value10;
real32 value11;
real32 value12;
real32 value13;
real32 value14;
real32 value15;
real32 value16;
real32 value17;
real32 value18;
real32 value19;
real32 value20;
real32 value21;
real32 value22;
real32 value23;
real32 value24;
real32 value25;
real32 value26;
real32 value27;
real32 value28;
real32 value29;
real32 value30;
real32 value31;
real32 value32;
real32 value33;
real32 value34;
real32 value35;
real32 value36;
real32 value37;
real32 value38;
real32 value39;
real32 value40;
real32 value41;
real32 value42;
real32 value43;
real32 value44;
real32 value45;
real32 value46;
real32 value47;
real32 value48;
real32 value49;
real32 value50;
real32 value51;
real32 value52;
real32 value53;
real32 value54;
real32 value55;
real32 value56;
real32 value57;
real32 value58;
real32 value59;
real32 value60;
real32 value61;
real32 value62;
real32 value63;
real32 value64;
real32 value65;
real32 value66;
real32 value67;
real32 value68;
real32 value69;
real32 value70;
real32 value71;
real32 value72;
real32 value73;
real32 value74;
real32 value75;
real32 value76;
real32 value77;
real32 value78;
real32 value79;
real32 value80;
real32 value81;
real32 value82;
real32 value83;
real32 value84;
real32 value85;
real32 value86;
real32 value87;
real32 value88;
real32 value89;
real32 value90;
real32 value91;
real32 value92;
real32 value93;
real32 value94;
real32 value95;
real32 value96;
real32 value97;
real32 value98;
real32 value99;
real32 value100;
real32 value101;
real32 value102;
real32 value103;
real32 value104;
real32 value105;
real32 value106;
real32 value107;
real32 value108;
real32 value109;
real32 value110;
real32 value111;
real32 value112;
real32 value113;
real32 value114;
real32 value115;
real32 value116;
real32 value117;
real32 value118;
real32 value119;
real32 value120;
real32 value121;
real32 value122;
real32 value123;
real32 value124;
real32 value125;
real32 value126;
real32 value127;
real32 value128;
real32 value129;
real32 value130;
real32 value131;
real32 value132;
real32 value133;
real32 value134;
real32 value135;
real32 value136;
real32 value137;
real32 value138;
real32 value139;
real32 value140;
real32 value141;
real32 value142;
real32 value143;
real32 value144;
real32 value145;
real32 value146;
real32 value147;
real32 value148;
real32 value149;
real32 value150;
real32 value151;
real32 value152;
real32 value153;
real32 value154;
real32 value155;
real32 value156;
real32 value157;
real32 value158;
real32 value159;
real32 value160;
real32 value161;
real32 value162;
real32 value163;
real32 value164;
real32 value165;
real32 value166;
real32 value167;
real32 value168;
real32 value169;
real32 value170;
real32 value171;
real32 value172;
real32 value173;
real32 value174;
real32 value175;
real32 value176;
real32 value177;
real32 value178;
real32 value179;
real32 value180;
real32 value181;
real32 value182;
real32 value183;
real32 value184;
real32 value185;
real32 value186;
real32 value187;
real32 value188;
real32 value189;
real32 value190;
real32 value191;
real32 value192;
real32 value193;
real32 value194;
real32 value195;
real32 value196;
real32 value197;
real32 value198;
real32 value199;
real32 value200;
real32 value201;
real32 value202;
real32 value203;
real32 value204;
real32 value205;
real32 value206;
real32 value207;
real32 value208;
real32 value209;
real32 value210;
real32 value211;
real32 value212;
real32 value213;
real32 value214;
real32 value215;
real32 value216;
real32 value217;
real32 value218;
real32 value219;
real32 value220;
real32 value221;
real32 value222;
real32 value223;
real32 value224;
real32 value225;
real32 value226;
real32 value227;
real32 value228;
real32 value229;
real32 value230;
real32 value231;
real32 value232;
real32 value233;
real32 value234;
real32 value235;
real32 value236;
real32 value237;
real32 value238;
real32 value239;
real32 value240;
real32 value241;
real32 value242;
real32 value243;
real32 value244;
real32 value245;
real32 value246;
real32 value247;
real32 value248;
real32 value249;
real32 value250;
real32 value251;
real32 value252;
real32 value253;
real32 value254;
real32 value255;
real32 value256;
real32 value257;
real32 value258;
real32 value259;
real32 value260;
real32 value261;
real32 value262;
real32 value263;
real32 value264;
real32 value265;
real32 value266;
real32 value267;
real32 value268;
real32 value269;
real32 value270;
real32 value271;
real32 value272;
real32 value273;
real32 value274;
real32 value275;
real32 value276;
real32 value277;
real32 value278;
real32 value279;
real32 value280;
real32 value281;
real32 value282;
real32 value283;
real32 value284;
real32 value285;
real32 value286;
real32 value287;
real32 value288;
real32 value289;
real32 value290;
real32 value291;
real32 value292;
real32 value293;
real32 value294;
real32 value295;
real32 value296;
real32 value297;
real32 value298;
real32 value299;
real32 value300;
real32 value301;
real32 value302;
real32 value303;
real32 value304;
real32 value305;
real32 value306;
real32 value307;
real32 value308;
real32 value309;
real32 value310;
real32 value311;
real32 value312;
real32 value313;
real32 value314;
real32 value315;
real32 value316;
real32 value317;
real32 value318;
real32 value319;
real32 value320;
real32 value321;
real32 value322;
real32 value323;
real32 value324;
real32 value325;
real32 value326;
real32 value327;
real32 value328;
real32 value329;
real32 value330;
real32 value331;
real32 value332;
real32 value333;
real32 value334;
real32 value335;
real32 value336;
real32 value337;
real32 value338;
real32 value339;
real32 value340;
real32 value341;
real32 value342;
real32 value343;
real32 value344;
real32 value345;
real32 value346;
real32 value347;
real32 value348;
real32 value349;
real32 value350;
real32 value351;
real32 value352;
real32 value353;
real32 value354;
real32 value355;
real32 value356;
real32 value357;
real32 value358;
real32 value359;
real32 value360;
real32 value361;
real32 value362;
real32 value363;
real32 value364;
real32 value365;
real32 value366;
real32 value367;
real32 value368;
real32 value369;
real32 value370;
real32 value371;
real32 value372;
real32 value373;
real32 value374;
real32 value375;
real32 value376;
real32 value377;
real32 value378;
real32 value379;
real32 value380;
real32 value381;
real32 value382;
real32 value383;
real32 value384;
real32 value385;
real32 value386;
real32 value387;
real32 value388;
real32 value389;
real32 value390;
real32 value391;
real32 value392;
real32 value393;
real32 value394;
real32 value395;
real32 value396;
real32 value397;
real32 value398;
real32 value399;
real32 value400;
real32 value401;
real32 value402;
real32 value403;
real32 value404;
real32 value405;
real32 value406;
real32 value407;
real32 value408;
real32 value409;
real32 value410;
real32 value411;
real32 value412;
real32 value413;
real32 value414;
real32 value415;
real32 value416;
real32 value417;
real32 value418;
real32 value419;
real32 value420;
real32 value421;
real32 value422;
real32 value423;
real32 value424;
real32 value425;
real32 value426;
real32 value427;
real32 value428;
real32 value429;
real32 value430;
real32 value431;
real32 value432;
real32 value433;
real32 value434;
real32 value435;
real32 value436;
real32 value437;
real32 value438;
real32 value439;
real32 value440;
real32 value441;
real32 value442;
real32 value443;
real32 value444;
real32 value445;
real32 value446;
real32 value447;
real32 value448;
real32 value449;
real32 value450;
real32 value451;
real32 value452;
real32 value453;
real32 value454;
real32 value455;
real32 value456;
real32 value457;
real32 value458;
real32 value459;
real32 value460;
real32 value461;
real32 value462;
real32 value463;
real32 value464;
real32 value465;
real32 value466;
real32 value467;
real32 value468;
real32 value469;
real32 value470;
real32 value471;
real32 value472;
real32 value473;
real32 value474;
real32 value475;
real32 value476;
real32 value477;
real32 value478;
real32 value479;
real32 value480;
real32 value481;
real32 value482;
real32 value483;
real32 value484;
real32 value485;
real32 value486;
real32 value487;
real32 value488;
real32 value489;
real32 value490;
real32 value491;
real32 value492;
real32 value493;
real32 value494;
real32 value495;
real32 value496;
real32 value497;
real32 value498;
real32 value499;
real32 value500;
real32 value501;
real32 value502;
real32 value503;
real32 value504;
real32 value505;
real32 value506;
real32 value507;
real32 value508;
real32 value509;
real32 value510;
real32 value511;
real32 value512;
real32 value513;
real32 value514;
real32 value515;
real32 value516;
real32 value517;
real32 value518;
real32 value519;
real32 value520;
real32 value521;
real32 value522;
real32 value523;
real32 value524;
real32 value525;
real32 value526;
real32 value527;
real32 value528;
real32 value529;
real32 value530;
real32 value531;
real32 value532;
real32 value533;
real32 value534;
real32 value535;
real32 value536;
real32 value537;
real32 value538;
real32 value539;
real32 value540;
real32 value541;
real32 value542;
real32 value543;
real32 value544;
real32 value545;
real32 value546;
real32 value547;
real32 value548;
real32 value549;
real32 value550;
real32 value551;
real32 value552;
real32 value553;
real32 value554;
real32 value555;
real32 value556;
real32 value557;
real32 value558;
real32 value559;
real32 value560;
real32 value561;
real32 value562;
real32 value563;
real32 value564;
real32 value565;
real32 value566;
real32 value567;
real32 value568;
real32 value569;
real32 value570;
real32 value571;
real32 value572;
real32 value573;
real32 value574;
real32 value575;
real32 value576;
real32 value577;
real32 value578;
real32 value579;
real32 value580;
real32 value581;
real32 value582;
real32 value583;
real32 value584;
real32 value585;
real32 value586;
real32 value587;
real32 value588;
real32 value589;
real32 value590;
real32 value591;
real32 value592;
real32 value593;
real32 value594;
real32 value595;
real32 value596;
real32 value597;
real32 value598;
real32 value599;
real32 value600;
real32 value601;
real32 value602;
real32 value603;
real32 value604;
real32 value605;
real32 value606;
real32 value607;
real32 value608;
real32 value609;
real32 value610;
real32 value611;
real32 value612;
real32 value613;
real32 value614;
real32 value615;
real32 value616;
real32 value617;
real32 value618;
real32 value619;
real32 value620;
real32 value621;
real32 value622;
real32 value623;
real32 value624;
real32 value625;
real32 value626;
real32 value627;
real32 value628;
real32 value629;
real32 value630;
real32 value631;
real32 value632;
real32 value633;
real32 value634;
real32 value635;
real32 value636;
real32 value637;
real32 value638;
real32 value639;
real32 value640;
real32 value641;
real32 value642;
real32 value643;
real32 value644;
real32 value645;
real32 value646;
real32 value647;
real32 value648;
real32 value649;
real32 value650;
real32 value651;
real32 value652;
real32 value653;
real32 value654;
real32 value655;
real32 value656;
real32 value657;
real32 value658;
real32 value659;
real32 value660;
real32 value661;
real32 value662;
real32 value663;
real32 value664;
real32 value665;
real32 value666;
real32 value667;
real32 value668;
real32 value669;
real32 value670;
real32 value671;
real32 value672;
real32 value673;
real32 value674;
real32 value675;
real32 value676;
real32 value677;
real32 value678;
real32 value679;
real32 value680;
real32 value681;
real32 value682;
real32 value683;
real32 value684;
real32 value685;
real32 value686;
real32 value687;
real32 value688;
real32 value689;
real32 value690;
real32 value691;
real32 value692;
real32 value693;
real32 value694;
real32 value695;
real32 value696;
real32 value697;
real32 value698;
real32 value699;
real32 value700;
real32 value701;
real32 value702;
real32 value703;
real32 value704;
real32 value705;
real32 value706;
real32 value707;
real32 value708;
real32 value709;
real32 value710;
real32 value711;
real32 value712;
real32 value713;
real32 value714;
real32 value715;
real32 value716;
real32 value717;
real32 value718;
real32 value719;
real32 value720;
real32 value721;
real32 value722;
real32 value723;
real32 value724;
real32 value725;
real32 value726;
real32 value727;
real32 value728;
real32 value729;
real32 value730;
real32 value731;
real32 value732;
real32 value733;
real32 value734;
real32 value735;
real32 value736;
real32 value737;
real32 value738;
real32 value739;
real32 value740;
real32 value741;
real32 value742;
real32 value743;
real32 value744;
real32 value745;
real32 value746;
real32 value747;
real32 value748;
real32 value749;
real32 value750;
real32 value751;
real32 value752;
real32 value753;
real32 value754;
real32 value755;
real32 value756;
real32 value757;
real32 value758;
real32 value759;
real32 value760;
real32 value761;
real32 value762;
real32 value763;
real32 value764;
real32 value765;
real32 value766;
real32 value767;
real32 value768;
real32 value769;
real32 value770;
real32 value771;
real32 value772;
real32 value773;
real32 value774;
real32 value775;
real32 value776;
real32 value777;
real32 value778;
real32 value779;
real32 value780;
real32 value781;
real32 value782;
real32 value783;
real32 value784;
real32 value785;
real32 value786;
real32 value787;
real32 value788;
real32 value789;
real32 value790;
real32 value791;
real32 value792;
real32 value793;
real32 value794;
real32 value795;
real32 value796;
real32 value797;
real32 value798;
real32 value799;
real32 value800;
real32 value801;
real32 value802;
real32 value803;
real32 value804;
real32 value805;
real32 value806;
real32 value807;
real32 value808;
real32 value809;
real32 value810;
real32 value811;
real32 value812;
real32 value813;
real32 value814;
real32 value815;
real32 value816;
real32 value817;
real32 value818;
real32 value819;
real32 value820;
real32 value821;
real32 value822;
real32 value823;
real32 value824;
real32 value825;
real32 value826;
real32 value827;
real32 value828;
real32 value829;
real32 value830;
real32 value831;
real32 value832;
real32 value833;
real32 value834;
real32 value835;
real32 value836;
real32 value837;
real32 value838;
real32 value839;
real32 value840;
real32 value841;
real32 value842;
real32 value843;
real32 value844;
real32 value845;
real32 value846;
real32 value847;
real32 value848;
real32 value849;
real32 value850;
real32 value851;
real32 value852;
real32 value853;
real32 value854;
real32 value855;
real32 value856;
real32 value857;
real32 value858;
real32 value859;
real32 value860;
real32 value861;
real32 value862;
real32 value863;
real32 value864;
real32 value865;
real32 value866;
real32 value867;
real32 value868;
real32 value869;
real32 value870;
real32 value871;
real32 value872;
real32 value873;
real32 value874;
real32 value875;
real32 value876;
real32 value877;
real32 value878;
real32 value879;
real32 value880;
real32 value881;
real32 value882;
real32 value883;
real32 value884;
real32 value885;
real32 value886;
real32 value887;
real32 value888;
real32 value889;
real32 value890;
real32 value891;
real32 value892;
real32 value893;
real32 value894;
real32 value895;
real32 value896;
real32 value897;
real32 value898;
real32 value899;
real32 value900;
real32 value901;
real32 value902;
real32 value903;
real32 value904;
real32 value905;
real32 value906;
real32 value907;
real32 value908;
real32 value909;
real32 value910;
real32 value911;
real32 value912;
real32 value913;
real32 value914;
real32 value915;
real32 value916;
real32 value917;
real32 value918;
real32 value919;
real32 value920;
real32 value921;
real32 value922;
real32 value923;
real32 value924;
real32 value925;
real32 value926;
real32 value927;
real32 value928;
real32 value929;
real32 value930;
real32 value931;
real32 value932;
real32 value933;
real32 value934;
real32 value935;
real32 value936;
real32 value937;
real32 value938;
real32 value939;
real32 value940;
real32 value941;
real32 value942;
real32 value943;
real32 value944;
real32 value945;
real32 value946;
real32 value947;
real32 value948;
real32 value949;
real32 value950;
real32 value951;
real32 value952;
real32 value953;
real32 value954;
real32 value955;
real32 value956;
real32 value957;
real32 value958;
real32 value959;
real32 value960;
real32 value961;
real32 value962;
real32 value963;
real32 value964;
real32 value965;
real32 value966;
real32 value967;
real32 value968;
real32 value969;
real32 value970;
real32 value971;
real32 value972;
real32 value973;
real32 value974;
real32 value975;
real32 value976;
real32 value977;
real32 value978;
real32 value979;
real32 value980;
real32 value981;
real32 value982;
real32 value983;
real32 value984;
real32 value985;
real32 value986;
real32 value987;
real32 value988;
real32 value989;
real32 value990;
real32 value991;
real32 value992;
real32 value993;
real32 value994;
real32 value995;
real32 value996;
real32 value997;
real32 value998;
real32 value999;
real32 value1000;
real32 value1001;
real32 value1002;
real32 value1003;
real32 value1004;
real32 value1005;
real32 value1006;
real32 value1007;
real32 value1008;
real32 value1009;
real32 value1010;
real32 value1011;
real32 value1012;
real32 value1013;
real32 value1014;
real32 value1015;
real32 value1016;
real32 value1017;
real32 value1018;
real32 value1019;
real32 value1020;
real32 value1021;
real32 value1022;
real32 value1023;
real32 value1024;
real32 value1025;

myfile >> value1;
myfile >> value2;
myfile >> value3;
myfile >> value4;
myfile >> value5;
myfile >> value6;
myfile >> value7;
myfile >> value8;
myfile >> value9;
myfile >> value10;
myfile >> value11;
myfile >> value12;
myfile >> value13;
myfile >> value14;
myfile >> value15;
myfile >> value16;
myfile >> value17;
myfile >> value18;
myfile >> value19;
myfile >> value20;
myfile >> value21;
myfile >> value22;
myfile >> value23;
myfile >> value24;
myfile >> value25;
myfile >> value26;
myfile >> value27;
myfile >> value28;
myfile >> value29;
myfile >> value30;
myfile >> value31;
myfile >> value32;
myfile >> value33;
myfile >> value34;
myfile >> value35;
myfile >> value36;
myfile >> value37;
myfile >> value38;
myfile >> value39;
myfile >> value40;
myfile >> value41;
myfile >> value42;
myfile >> value43;
myfile >> value44;
myfile >> value45;
myfile >> value46;
myfile >> value47;
myfile >> value48;
myfile >> value49;
myfile >> value50;
myfile >> value51;
myfile >> value52;
myfile >> value53;
myfile >> value54;
myfile >> value55;
myfile >> value56;
myfile >> value57;
myfile >> value58;
myfile >> value59;
myfile >> value60;
myfile >> value61;
myfile >> value62;
myfile >> value63;
myfile >> value64;
myfile >> value65;
myfile >> value66;
myfile >> value67;
myfile >> value68;
myfile >> value69;
myfile >> value70;
myfile >> value71;
myfile >> value72;
myfile >> value73;
myfile >> value74;
myfile >> value75;
myfile >> value76;
myfile >> value77;
myfile >> value78;
myfile >> value79;
myfile >> value80;
myfile >> value81;
myfile >> value82;
myfile >> value83;
myfile >> value84;
myfile >> value85;
myfile >> value86;
myfile >> value87;
myfile >> value88;
myfile >> value89;
myfile >> value90;
myfile >> value91;
myfile >> value92;
myfile >> value93;
myfile >> value94;
myfile >> value95;
myfile >> value96;
myfile >> value97;
myfile >> value98;
myfile >> value99;
myfile >> value100;
myfile >> value101;
myfile >> value102;
myfile >> value103;
myfile >> value104;
myfile >> value105;
myfile >> value106;
myfile >> value107;
myfile >> value108;
myfile >> value109;
myfile >> value110;
myfile >> value111;
myfile >> value112;
myfile >> value113;
myfile >> value114;
myfile >> value115;
myfile >> value116;
myfile >> value117;
myfile >> value118;
myfile >> value119;
myfile >> value120;
myfile >> value121;
myfile >> value122;
myfile >> value123;
myfile >> value124;
myfile >> value125;
myfile >> value126;
myfile >> value127;
myfile >> value128;
myfile >> value129;
myfile >> value130;
myfile >> value131;
myfile >> value132;
myfile >> value133;
myfile >> value134;
myfile >> value135;
myfile >> value136;
myfile >> value137;
myfile >> value138;
myfile >> value139;
myfile >> value140;
myfile >> value141;
myfile >> value142;
myfile >> value143;
myfile >> value144;
myfile >> value145;
myfile >> value146;
myfile >> value147;
myfile >> value148;
myfile >> value149;
myfile >> value150;
myfile >> value151;
myfile >> value152;
myfile >> value153;
myfile >> value154;
myfile >> value155;
myfile >> value156;
myfile >> value157;
myfile >> value158;
myfile >> value159;
myfile >> value160;
myfile >> value161;
myfile >> value162;
myfile >> value163;
myfile >> value164;
myfile >> value165;
myfile >> value166;
myfile >> value167;
myfile >> value168;
myfile >> value169;
myfile >> value170;
myfile >> value171;
myfile >> value172;
myfile >> value173;
myfile >> value174;
myfile >> value175;
myfile >> value176;
myfile >> value177;
myfile >> value178;
myfile >> value179;
myfile >> value180;
myfile >> value181;
myfile >> value182;
myfile >> value183;
myfile >> value184;
myfile >> value185;
myfile >> value186;
myfile >> value187;
myfile >> value188;
myfile >> value189;
myfile >> value190;
myfile >> value191;
myfile >> value192;
myfile >> value193;
myfile >> value194;
myfile >> value195;
myfile >> value196;
myfile >> value197;
myfile >> value198;
myfile >> value199;
myfile >> value200;
myfile >> value201;
myfile >> value202;
myfile >> value203;
myfile >> value204;
myfile >> value205;
myfile >> value206;
myfile >> value207;
myfile >> value208;
myfile >> value209;
myfile >> value210;
myfile >> value211;
myfile >> value212;
myfile >> value213;
myfile >> value214;
myfile >> value215;
myfile >> value216;
myfile >> value217;
myfile >> value218;
myfile >> value219;
myfile >> value220;
myfile >> value221;
myfile >> value222;
myfile >> value223;
myfile >> value224;
myfile >> value225;
myfile >> value226;
myfile >> value227;
myfile >> value228;
myfile >> value229;
myfile >> value230;
myfile >> value231;
myfile >> value232;
myfile >> value233;
myfile >> value234;
myfile >> value235;
myfile >> value236;
myfile >> value237;
myfile >> value238;
myfile >> value239;
myfile >> value240;
myfile >> value241;
myfile >> value242;
myfile >> value243;
myfile >> value244;
myfile >> value245;
myfile >> value246;
myfile >> value247;
myfile >> value248;
myfile >> value249;
myfile >> value250;
myfile >> value251;
myfile >> value252;
myfile >> value253;
myfile >> value254;
myfile >> value255;
myfile >> value256;
myfile >> value257;
myfile >> value258;
myfile >> value259;
myfile >> value260;
myfile >> value261;
myfile >> value262;
myfile >> value263;
myfile >> value264;
myfile >> value265;
myfile >> value266;
myfile >> value267;
myfile >> value268;
myfile >> value269;
myfile >> value270;
myfile >> value271;
myfile >> value272;
myfile >> value273;
myfile >> value274;
myfile >> value275;
myfile >> value276;
myfile >> value277;
myfile >> value278;
myfile >> value279;
myfile >> value280;
myfile >> value281;
myfile >> value282;
myfile >> value283;
myfile >> value284;
myfile >> value285;
myfile >> value286;
myfile >> value287;
myfile >> value288;
myfile >> value289;
myfile >> value290;
myfile >> value291;
myfile >> value292;
myfile >> value293;
myfile >> value294;
myfile >> value295;
myfile >> value296;
myfile >> value297;
myfile >> value298;
myfile >> value299;
myfile >> value300;
myfile >> value301;
myfile >> value302;
myfile >> value303;
myfile >> value304;
myfile >> value305;
myfile >> value306;
myfile >> value307;
myfile >> value308;
myfile >> value309;
myfile >> value310;
myfile >> value311;
myfile >> value312;
myfile >> value313;
myfile >> value314;
myfile >> value315;
myfile >> value316;
myfile >> value317;
myfile >> value318;
myfile >> value319;
myfile >> value320;
myfile >> value321;
myfile >> value322;
myfile >> value323;
myfile >> value324;
myfile >> value325;
myfile >> value326;
myfile >> value327;
myfile >> value328;
myfile >> value329;
myfile >> value330;
myfile >> value331;
myfile >> value332;
myfile >> value333;
myfile >> value334;
myfile >> value335;
myfile >> value336;
myfile >> value337;
myfile >> value338;
myfile >> value339;
myfile >> value340;
myfile >> value341;
myfile >> value342;
myfile >> value343;
myfile >> value344;
myfile >> value345;
myfile >> value346;
myfile >> value347;
myfile >> value348;
myfile >> value349;
myfile >> value350;
myfile >> value351;
myfile >> value352;
myfile >> value353;
myfile >> value354;
myfile >> value355;
myfile >> value356;
myfile >> value357;
myfile >> value358;
myfile >> value359;
myfile >> value360;
myfile >> value361;
myfile >> value362;
myfile >> value363;
myfile >> value364;
myfile >> value365;
myfile >> value366;
myfile >> value367;
myfile >> value368;
myfile >> value369;
myfile >> value370;
myfile >> value371;
myfile >> value372;
myfile >> value373;
myfile >> value374;
myfile >> value375;
myfile >> value376;
myfile >> value377;
myfile >> value378;
myfile >> value379;
myfile >> value380;
myfile >> value381;
myfile >> value382;
myfile >> value383;
myfile >> value384;
myfile >> value385;
myfile >> value386;
myfile >> value387;
myfile >> value388;
myfile >> value389;
myfile >> value390;
myfile >> value391;
myfile >> value392;
myfile >> value393;
myfile >> value394;
myfile >> value395;
myfile >> value396;
myfile >> value397;
myfile >> value398;
myfile >> value399;
myfile >> value400;
myfile >> value401;
myfile >> value402;
myfile >> value403;
myfile >> value404;
myfile >> value405;
myfile >> value406;
myfile >> value407;
myfile >> value408;
myfile >> value409;
myfile >> value410;
myfile >> value411;
myfile >> value412;
myfile >> value413;
myfile >> value414;
myfile >> value415;
myfile >> value416;
myfile >> value417;
myfile >> value418;
myfile >> value419;
myfile >> value420;
myfile >> value421;
myfile >> value422;
myfile >> value423;
myfile >> value424;
myfile >> value425;
myfile >> value426;
myfile >> value427;
myfile >> value428;
myfile >> value429;
myfile >> value430;
myfile >> value431;
myfile >> value432;
myfile >> value433;
myfile >> value434;
myfile >> value435;
myfile >> value436;
myfile >> value437;
myfile >> value438;
myfile >> value439;
myfile >> value440;
myfile >> value441;
myfile >> value442;
myfile >> value443;
myfile >> value444;
myfile >> value445;
myfile >> value446;
myfile >> value447;
myfile >> value448;
myfile >> value449;
myfile >> value450;
myfile >> value451;
myfile >> value452;
myfile >> value453;
myfile >> value454;
myfile >> value455;
myfile >> value456;
myfile >> value457;
myfile >> value458;
myfile >> value459;
myfile >> value460;
myfile >> value461;
myfile >> value462;
myfile >> value463;
myfile >> value464;
myfile >> value465;
myfile >> value466;
myfile >> value467;
myfile >> value468;
myfile >> value469;
myfile >> value470;
myfile >> value471;
myfile >> value472;
myfile >> value473;
myfile >> value474;
myfile >> value475;
myfile >> value476;
myfile >> value477;
myfile >> value478;
myfile >> value479;
myfile >> value480;
myfile >> value481;
myfile >> value482;
myfile >> value483;
myfile >> value484;
myfile >> value485;
myfile >> value486;
myfile >> value487;
myfile >> value488;
myfile >> value489;
myfile >> value490;
myfile >> value491;
myfile >> value492;
myfile >> value493;
myfile >> value494;
myfile >> value495;
myfile >> value496;
myfile >> value497;
myfile >> value498;
myfile >> value499;
myfile >> value500;
myfile >> value501;
myfile >> value502;
myfile >> value503;
myfile >> value504;
myfile >> value505;
myfile >> value506;
myfile >> value507;
myfile >> value508;
myfile >> value509;
myfile >> value510;
myfile >> value511;
myfile >> value512;
myfile >> value513;
myfile >> value514;
myfile >> value515;
myfile >> value516;
myfile >> value517;
myfile >> value518;
myfile >> value519;
myfile >> value520;
myfile >> value521;
myfile >> value522;
myfile >> value523;
myfile >> value524;
myfile >> value525;
myfile >> value526;
myfile >> value527;
myfile >> value528;
myfile >> value529;
myfile >> value530;
myfile >> value531;
myfile >> value532;
myfile >> value533;
myfile >> value534;
myfile >> value535;
myfile >> value536;
myfile >> value537;
myfile >> value538;
myfile >> value539;
myfile >> value540;
myfile >> value541;
myfile >> value542;
myfile >> value543;
myfile >> value544;
myfile >> value545;
myfile >> value546;
myfile >> value547;
myfile >> value548;
myfile >> value549;
myfile >> value550;
myfile >> value551;
myfile >> value552;
myfile >> value553;
myfile >> value554;
myfile >> value555;
myfile >> value556;
myfile >> value557;
myfile >> value558;
myfile >> value559;
myfile >> value560;
myfile >> value561;
myfile >> value562;
myfile >> value563;
myfile >> value564;
myfile >> value565;
myfile >> value566;
myfile >> value567;
myfile >> value568;
myfile >> value569;
myfile >> value570;
myfile >> value571;
myfile >> value572;
myfile >> value573;
myfile >> value574;
myfile >> value575;
myfile >> value576;
myfile >> value577;
myfile >> value578;
myfile >> value579;
myfile >> value580;
myfile >> value581;
myfile >> value582;
myfile >> value583;
myfile >> value584;
myfile >> value585;
myfile >> value586;
myfile >> value587;
myfile >> value588;
myfile >> value589;
myfile >> value590;
myfile >> value591;
myfile >> value592;
myfile >> value593;
myfile >> value594;
myfile >> value595;
myfile >> value596;
myfile >> value597;
myfile >> value598;
myfile >> value599;
myfile >> value600;
myfile >> value601;
myfile >> value602;
myfile >> value603;
myfile >> value604;
myfile >> value605;
myfile >> value606;
myfile >> value607;
myfile >> value608;
myfile >> value609;
myfile >> value610;
myfile >> value611;
myfile >> value612;
myfile >> value613;
myfile >> value614;
myfile >> value615;
myfile >> value616;
myfile >> value617;
myfile >> value618;
myfile >> value619;
myfile >> value620;
myfile >> value621;
myfile >> value622;
myfile >> value623;
myfile >> value624;
myfile >> value625;
myfile >> value626;
myfile >> value627;
myfile >> value628;
myfile >> value629;
myfile >> value630;
myfile >> value631;
myfile >> value632;
myfile >> value633;
myfile >> value634;
myfile >> value635;
myfile >> value636;
myfile >> value637;
myfile >> value638;
myfile >> value639;
myfile >> value640;
myfile >> value641;
myfile >> value642;
myfile >> value643;
myfile >> value644;
myfile >> value645;
myfile >> value646;
myfile >> value647;
myfile >> value648;
myfile >> value649;
myfile >> value650;
myfile >> value651;
myfile >> value652;
myfile >> value653;
myfile >> value654;
myfile >> value655;
myfile >> value656;
myfile >> value657;
myfile >> value658;
myfile >> value659;
myfile >> value660;
myfile >> value661;
myfile >> value662;
myfile >> value663;
myfile >> value664;
myfile >> value665;
myfile >> value666;
myfile >> value667;
myfile >> value668;
myfile >> value669;
myfile >> value670;
myfile >> value671;
myfile >> value672;
myfile >> value673;
myfile >> value674;
myfile >> value675;
myfile >> value676;
myfile >> value677;
myfile >> value678;
myfile >> value679;
myfile >> value680;
myfile >> value681;
myfile >> value682;
myfile >> value683;
myfile >> value684;
myfile >> value685;
myfile >> value686;
myfile >> value687;
myfile >> value688;
myfile >> value689;
myfile >> value690;
myfile >> value691;
myfile >> value692;
myfile >> value693;
myfile >> value694;
myfile >> value695;
myfile >> value696;
myfile >> value697;
myfile >> value698;
myfile >> value699;
myfile >> value700;
myfile >> value701;
myfile >> value702;
myfile >> value703;
myfile >> value704;
myfile >> value705;
myfile >> value706;
myfile >> value707;
myfile >> value708;
myfile >> value709;
myfile >> value710;
myfile >> value711;
myfile >> value712;
myfile >> value713;
myfile >> value714;
myfile >> value715;
myfile >> value716;
myfile >> value717;
myfile >> value718;
myfile >> value719;
myfile >> value720;
myfile >> value721;
myfile >> value722;
myfile >> value723;
myfile >> value724;
myfile >> value725;
myfile >> value726;
myfile >> value727;
myfile >> value728;
myfile >> value729;
myfile >> value730;
myfile >> value731;
myfile >> value732;
myfile >> value733;
myfile >> value734;
myfile >> value735;
myfile >> value736;
myfile >> value737;
myfile >> value738;
myfile >> value739;
myfile >> value740;
myfile >> value741;
myfile >> value742;
myfile >> value743;
myfile >> value744;
myfile >> value745;
myfile >> value746;
myfile >> value747;
myfile >> value748;
myfile >> value749;
myfile >> value750;
myfile >> value751;
myfile >> value752;
myfile >> value753;
myfile >> value754;
myfile >> value755;
myfile >> value756;
myfile >> value757;
myfile >> value758;
myfile >> value759;
myfile >> value760;
myfile >> value761;
myfile >> value762;
myfile >> value763;
myfile >> value764;
myfile >> value765;
myfile >> value766;
myfile >> value767;
myfile >> value768;
myfile >> value769;
myfile >> value770;
myfile >> value771;
myfile >> value772;
myfile >> value773;
myfile >> value774;
myfile >> value775;
myfile >> value776;
myfile >> value777;
myfile >> value778;
myfile >> value779;
myfile >> value780;
myfile >> value781;
myfile >> value782;
myfile >> value783;
myfile >> value784;
myfile >> value785;
myfile >> value786;
myfile >> value787;
myfile >> value788;
myfile >> value789;
myfile >> value790;
myfile >> value791;
myfile >> value792;
myfile >> value793;
myfile >> value794;
myfile >> value795;
myfile >> value796;
myfile >> value797;
myfile >> value798;
myfile >> value799;
myfile >> value800;
myfile >> value801;
myfile >> value802;
myfile >> value803;
myfile >> value804;
myfile >> value805;
myfile >> value806;
myfile >> value807;
myfile >> value808;
myfile >> value809;
myfile >> value810;
myfile >> value811;
myfile >> value812;
myfile >> value813;
myfile >> value814;
myfile >> value815;
myfile >> value816;
myfile >> value817;
myfile >> value818;
myfile >> value819;
myfile >> value820;
myfile >> value821;
myfile >> value822;
myfile >> value823;
myfile >> value824;
myfile >> value825;
myfile >> value826;
myfile >> value827;
myfile >> value828;
myfile >> value829;
myfile >> value830;
myfile >> value831;
myfile >> value832;
myfile >> value833;
myfile >> value834;
myfile >> value835;
myfile >> value836;
myfile >> value837;
myfile >> value838;
myfile >> value839;
myfile >> value840;
myfile >> value841;
myfile >> value842;
myfile >> value843;
myfile >> value844;
myfile >> value845;
myfile >> value846;
myfile >> value847;
myfile >> value848;
myfile >> value849;
myfile >> value850;
myfile >> value851;
myfile >> value852;
myfile >> value853;
myfile >> value854;
myfile >> value855;
myfile >> value856;
myfile >> value857;
myfile >> value858;
myfile >> value859;
myfile >> value860;
myfile >> value861;
myfile >> value862;
myfile >> value863;
myfile >> value864;
myfile >> value865;
myfile >> value866;
myfile >> value867;
myfile >> value868;
myfile >> value869;
myfile >> value870;
myfile >> value871;
myfile >> value872;
myfile >> value873;
myfile >> value874;
myfile >> value875;
myfile >> value876;
myfile >> value877;
myfile >> value878;
myfile >> value879;
myfile >> value880;
myfile >> value881;
myfile >> value882;
myfile >> value883;
myfile >> value884;
myfile >> value885;
myfile >> value886;
myfile >> value887;
myfile >> value888;
myfile >> value889;
myfile >> value890;
myfile >> value891;
myfile >> value892;
myfile >> value893;
myfile >> value894;
myfile >> value895;
myfile >> value896;
myfile >> value897;
myfile >> value898;
myfile >> value899;
myfile >> value900;
myfile >> value901;
myfile >> value902;
myfile >> value903;
myfile >> value904;
myfile >> value905;
myfile >> value906;
myfile >> value907;
myfile >> value908;
myfile >> value909;
myfile >> value910;
myfile >> value911;
myfile >> value912;
myfile >> value913;
myfile >> value914;
myfile >> value915;
myfile >> value916;
myfile >> value917;
myfile >> value918;
myfile >> value919;
myfile >> value920;
myfile >> value921;
myfile >> value922;
myfile >> value923;
myfile >> value924;
myfile >> value925;
myfile >> value926;
myfile >> value927;
myfile >> value928;
myfile >> value929;
myfile >> value930;
myfile >> value931;
myfile >> value932;
myfile >> value933;
myfile >> value934;
myfile >> value935;
myfile >> value936;
myfile >> value937;
myfile >> value938;
myfile >> value939;
myfile >> value940;
myfile >> value941;
myfile >> value942;
myfile >> value943;
myfile >> value944;
myfile >> value945;
myfile >> value946;
myfile >> value947;
myfile >> value948;
myfile >> value949;
myfile >> value950;
myfile >> value951;
myfile >> value952;
myfile >> value953;
myfile >> value954;
myfile >> value955;
myfile >> value956;
myfile >> value957;
myfile >> value958;
myfile >> value959;
myfile >> value960;
myfile >> value961;
myfile >> value962;
myfile >> value963;
myfile >> value964;
myfile >> value965;
myfile >> value966;
myfile >> value967;
myfile >> value968;
myfile >> value969;
myfile >> value970;
myfile >> value971;
myfile >> value972;
myfile >> value973;
myfile >> value974;
myfile >> value975;
myfile >> value976;
myfile >> value977;
myfile >> value978;
myfile >> value979;
myfile >> value980;
myfile >> value981;
myfile >> value982;
myfile >> value983;
myfile >> value984;
myfile >> value985;
myfile >> value986;
myfile >> value987;
myfile >> value988;
myfile >> value989;
myfile >> value990;
myfile >> value991;
myfile >> value992;
myfile >> value993;
myfile >> value994;
myfile >> value995;
myfile >> value996;
myfile >> value997;
myfile >> value998;
myfile >> value999;
myfile >> value1000;
myfile >> value1001;
myfile >> value1002;
myfile >> value1003;
myfile >> value1004;
myfile >> value1005;
myfile >> value1006;
myfile >> value1007;
myfile >> value1008;
myfile >> value1009;
myfile >> value1010;
myfile >> value1011;
myfile >> value1012;
myfile >> value1013;
myfile >> value1014;
myfile >> value1015;
myfile >> value1016;
myfile >> value1017;
myfile >> value1018;
myfile >> value1019;
myfile >> value1020;
myfile >> value1021;
myfile >> value1022;
myfile >> value1023;
myfile >> value1024;
myfile >> value1025;


	static const real32 kTable [] =
		{
value1,
value2,
value3,
value4,
value5,
value6,
value7,
value8,
value9,
value10,
value11,
value12,
value13,
value14,
value15,
value16,
value17,
value18,
value19,
value20,
value21,
value22,
value23,
value24,
value25,
value26,
value27,
value28,
value29,
value30,
value31,
value32,
value33,
value34,
value35,
value36,
value37,
value38,
value39,
value40,
value41,
value42,
value43,
value44,
value45,
value46,
value47,
value48,
value49,
value50,
value51,
value52,
value53,
value54,
value55,
value56,
value57,
value58,
value59,
value60,
value61,
value62,
value63,
value64,
value65,
value66,
value67,
value68,
value69,
value70,
value71,
value72,
value73,
value74,
value75,
value76,
value77,
value78,
value79,
value80,
value81,
value82,
value83,
value84,
value85,
value86,
value87,
value88,
value89,
value90,
value91,
value92,
value93,
value94,
value95,
value96,
value97,
value98,
value99,
value100,
value101,
value102,
value103,
value104,
value105,
value106,
value107,
value108,
value109,
value110,
value111,
value112,
value113,
value114,
value115,
value116,
value117,
value118,
value119,
value120,
value121,
value122,
value123,
value124,
value125,
value126,
value127,
value128,
value129,
value130,
value131,
value132,
value133,
value134,
value135,
value136,
value137,
value138,
value139,
value140,
value141,
value142,
value143,
value144,
value145,
value146,
value147,
value148,
value149,
value150,
value151,
value152,
value153,
value154,
value155,
value156,
value157,
value158,
value159,
value160,
value161,
value162,
value163,
value164,
value165,
value166,
value167,
value168,
value169,
value170,
value171,
value172,
value173,
value174,
value175,
value176,
value177,
value178,
value179,
value180,
value181,
value182,
value183,
value184,
value185,
value186,
value187,
value188,
value189,
value190,
value191,
value192,
value193,
value194,
value195,
value196,
value197,
value198,
value199,
value200,
value201,
value202,
value203,
value204,
value205,
value206,
value207,
value208,
value209,
value210,
value211,
value212,
value213,
value214,
value215,
value216,
value217,
value218,
value219,
value220,
value221,
value222,
value223,
value224,
value225,
value226,
value227,
value228,
value229,
value230,
value231,
value232,
value233,
value234,
value235,
value236,
value237,
value238,
value239,
value240,
value241,
value242,
value243,
value244,
value245,
value246,
value247,
value248,
value249,
value250,
value251,
value252,
value253,
value254,
value255,
value256,
value257,
value258,
value259,
value260,
value261,
value262,
value263,
value264,
value265,
value266,
value267,
value268,
value269,
value270,
value271,
value272,
value273,
value274,
value275,
value276,
value277,
value278,
value279,
value280,
value281,
value282,
value283,
value284,
value285,
value286,
value287,
value288,
value289,
value290,
value291,
value292,
value293,
value294,
value295,
value296,
value297,
value298,
value299,
value300,
value301,
value302,
value303,
value304,
value305,
value306,
value307,
value308,
value309,
value310,
value311,
value312,
value313,
value314,
value315,
value316,
value317,
value318,
value319,
value320,
value321,
value322,
value323,
value324,
value325,
value326,
value327,
value328,
value329,
value330,
value331,
value332,
value333,
value334,
value335,
value336,
value337,
value338,
value339,
value340,
value341,
value342,
value343,
value344,
value345,
value346,
value347,
value348,
value349,
value350,
value351,
value352,
value353,
value354,
value355,
value356,
value357,
value358,
value359,
value360,
value361,
value362,
value363,
value364,
value365,
value366,
value367,
value368,
value369,
value370,
value371,
value372,
value373,
value374,
value375,
value376,
value377,
value378,
value379,
value380,
value381,
value382,
value383,
value384,
value385,
value386,
value387,
value388,
value389,
value390,
value391,
value392,
value393,
value394,
value395,
value396,
value397,
value398,
value399,
value400,
value401,
value402,
value403,
value404,
value405,
value406,
value407,
value408,
value409,
value410,
value411,
value412,
value413,
value414,
value415,
value416,
value417,
value418,
value419,
value420,
value421,
value422,
value423,
value424,
value425,
value426,
value427,
value428,
value429,
value430,
value431,
value432,
value433,
value434,
value435,
value436,
value437,
value438,
value439,
value440,
value441,
value442,
value443,
value444,
value445,
value446,
value447,
value448,
value449,
value450,
value451,
value452,
value453,
value454,
value455,
value456,
value457,
value458,
value459,
value460,
value461,
value462,
value463,
value464,
value465,
value466,
value467,
value468,
value469,
value470,
value471,
value472,
value473,
value474,
value475,
value476,
value477,
value478,
value479,
value480,
value481,
value482,
value483,
value484,
value485,
value486,
value487,
value488,
value489,
value490,
value491,
value492,
value493,
value494,
value495,
value496,
value497,
value498,
value499,
value500,
value501,
value502,
value503,
value504,
value505,
value506,
value507,
value508,
value509,
value510,
value511,
value512,
value513,
value514,
value515,
value516,
value517,
value518,
value519,
value520,
value521,
value522,
value523,
value524,
value525,
value526,
value527,
value528,
value529,
value530,
value531,
value532,
value533,
value534,
value535,
value536,
value537,
value538,
value539,
value540,
value541,
value542,
value543,
value544,
value545,
value546,
value547,
value548,
value549,
value550,
value551,
value552,
value553,
value554,
value555,
value556,
value557,
value558,
value559,
value560,
value561,
value562,
value563,
value564,
value565,
value566,
value567,
value568,
value569,
value570,
value571,
value572,
value573,
value574,
value575,
value576,
value577,
value578,
value579,
value580,
value581,
value582,
value583,
value584,
value585,
value586,
value587,
value588,
value589,
value590,
value591,
value592,
value593,
value594,
value595,
value596,
value597,
value598,
value599,
value600,
value601,
value602,
value603,
value604,
value605,
value606,
value607,
value608,
value609,
value610,
value611,
value612,
value613,
value614,
value615,
value616,
value617,
value618,
value619,
value620,
value621,
value622,
value623,
value624,
value625,
value626,
value627,
value628,
value629,
value630,
value631,
value632,
value633,
value634,
value635,
value636,
value637,
value638,
value639,
value640,
value641,
value642,
value643,
value644,
value645,
value646,
value647,
value648,
value649,
value650,
value651,
value652,
value653,
value654,
value655,
value656,
value657,
value658,
value659,
value660,
value661,
value662,
value663,
value664,
value665,
value666,
value667,
value668,
value669,
value670,
value671,
value672,
value673,
value674,
value675,
value676,
value677,
value678,
value679,
value680,
value681,
value682,
value683,
value684,
value685,
value686,
value687,
value688,
value689,
value690,
value691,
value692,
value693,
value694,
value695,
value696,
value697,
value698,
value699,
value700,
value701,
value702,
value703,
value704,
value705,
value706,
value707,
value708,
value709,
value710,
value711,
value712,
value713,
value714,
value715,
value716,
value717,
value718,
value719,
value720,
value721,
value722,
value723,
value724,
value725,
value726,
value727,
value728,
value729,
value730,
value731,
value732,
value733,
value734,
value735,
value736,
value737,
value738,
value739,
value740,
value741,
value742,
value743,
value744,
value745,
value746,
value747,
value748,
value749,
value750,
value751,
value752,
value753,
value754,
value755,
value756,
value757,
value758,
value759,
value760,
value761,
value762,
value763,
value764,
value765,
value766,
value767,
value768,
value769,
value770,
value771,
value772,
value773,
value774,
value775,
value776,
value777,
value778,
value779,
value780,
value781,
value782,
value783,
value784,
value785,
value786,
value787,
value788,
value789,
value790,
value791,
value792,
value793,
value794,
value795,
value796,
value797,
value798,
value799,
value800,
value801,
value802,
value803,
value804,
value805,
value806,
value807,
value808,
value809,
value810,
value811,
value812,
value813,
value814,
value815,
value816,
value817,
value818,
value819,
value820,
value821,
value822,
value823,
value824,
value825,
value826,
value827,
value828,
value829,
value830,
value831,
value832,
value833,
value834,
value835,
value836,
value837,
value838,
value839,
value840,
value841,
value842,
value843,
value844,
value845,
value846,
value847,
value848,
value849,
value850,
value851,
value852,
value853,
value854,
value855,
value856,
value857,
value858,
value859,
value860,
value861,
value862,
value863,
value864,
value865,
value866,
value867,
value868,
value869,
value870,
value871,
value872,
value873,
value874,
value875,
value876,
value877,
value878,
value879,
value880,
value881,
value882,
value883,
value884,
value885,
value886,
value887,
value888,
value889,
value890,
value891,
value892,
value893,
value894,
value895,
value896,
value897,
value898,
value899,
value900,
value901,
value902,
value903,
value904,
value905,
value906,
value907,
value908,
value909,
value910,
value911,
value912,
value913,
value914,
value915,
value916,
value917,
value918,
value919,
value920,
value921,
value922,
value923,
value924,
value925,
value926,
value927,
value928,
value929,
value930,
value931,
value932,
value933,
value934,
value935,
value936,
value937,
value938,
value939,
value940,
value941,
value942,
value943,
value944,
value945,
value946,
value947,
value948,
value949,
value950,
value951,
value952,
value953,
value954,
value955,
value956,
value957,
value958,
value959,
value960,
value961,
value962,
value963,
value964,
value965,
value966,
value967,
value968,
value969,
value970,
value971,
value972,
value973,
value974,
value975,
value976,
value977,
value978,
value979,
value980,
value981,
value982,
value983,
value984,
value985,
value986,
value987,
value988,
value989,
value990,
value991,
value992,
value993,
value994,
value995,
value996,
value997,
value998,
value999,
value1000,
value1001,
value1002,
value1003,
value1004,
value1005,
value1006,
value1007,
value1008,
value1009,
value1010,
value1011,
value1012,
value1013,
value1014,
value1015,
value1016,
value1017,
value1018,
value1019,
value1020,
value1021,
value1022,
value1023,
value1024,
value1025,
		};
		
	const uint32 kTableSize = sizeof (kTable    ) /
							  sizeof (kTable [0]);
		
	real32 y = (real32) x * (real32) (kTableSize - 1);
	
	int32 index = Pin_int32 (0, (int32) y, kTableSize - 2);
	
	real32 fract = y - (real32) index;
	
	return kTable [index    ] * (1.0f - fract) +
		   kTable [index + 1] * (       fract);
	
	}

/*****************************************************************************/

real64 dng_tone_curve_acr3_default::EvaluateInverse (real64 x) const
	{
	
	static const real32 kTable [] =
		{
		0.00000f, 0.00121f, 0.00237f, 0.00362f,
		0.00496f, 0.00621f, 0.00738f, 0.00848f,
		0.00951f, 0.01048f, 0.01139f, 0.01227f,
		0.01312f, 0.01393f, 0.01471f, 0.01547f,
		0.01620f, 0.01692f, 0.01763f, 0.01831f,
		0.01899f, 0.01965f, 0.02030f, 0.02094f,
		0.02157f, 0.02218f, 0.02280f, 0.02340f,
		0.02399f, 0.02458f, 0.02517f, 0.02574f,
		0.02631f, 0.02688f, 0.02744f, 0.02800f,
		0.02855f, 0.02910f, 0.02965f, 0.03019f,
		0.03072f, 0.03126f, 0.03179f, 0.03232f,
		0.03285f, 0.03338f, 0.03390f, 0.03442f,
		0.03493f, 0.03545f, 0.03596f, 0.03647f,
		0.03698f, 0.03749f, 0.03799f, 0.03849f,
		0.03899f, 0.03949f, 0.03998f, 0.04048f,
		0.04097f, 0.04146f, 0.04195f, 0.04244f,
		0.04292f, 0.04341f, 0.04389f, 0.04437f,
		0.04485f, 0.04533f, 0.04580f, 0.04628f,
		0.04675f, 0.04722f, 0.04769f, 0.04816f,
		0.04863f, 0.04910f, 0.04956f, 0.05003f,
		0.05049f, 0.05095f, 0.05141f, 0.05187f,
		0.05233f, 0.05278f, 0.05324f, 0.05370f,
		0.05415f, 0.05460f, 0.05505f, 0.05551f,
		0.05595f, 0.05640f, 0.05685f, 0.05729f,
		0.05774f, 0.05818f, 0.05863f, 0.05907f,
		0.05951f, 0.05995f, 0.06039f, 0.06083f,
		0.06126f, 0.06170f, 0.06214f, 0.06257f,
		0.06301f, 0.06344f, 0.06388f, 0.06431f,
		0.06474f, 0.06517f, 0.06560f, 0.06602f,
		0.06645f, 0.06688f, 0.06731f, 0.06773f,
		0.06815f, 0.06858f, 0.06900f, 0.06943f,
		0.06985f, 0.07027f, 0.07069f, 0.07111f,
		0.07152f, 0.07194f, 0.07236f, 0.07278f,
		0.07319f, 0.07361f, 0.07402f, 0.07444f,
		0.07485f, 0.07526f, 0.07567f, 0.07608f,
		0.07650f, 0.07691f, 0.07732f, 0.07772f,
		0.07813f, 0.07854f, 0.07895f, 0.07935f,
		0.07976f, 0.08016f, 0.08057f, 0.08098f,
		0.08138f, 0.08178f, 0.08218f, 0.08259f,
		0.08299f, 0.08339f, 0.08379f, 0.08419f,
		0.08459f, 0.08499f, 0.08539f, 0.08578f,
		0.08618f, 0.08657f, 0.08697f, 0.08737f,
		0.08776f, 0.08816f, 0.08855f, 0.08894f,
		0.08934f, 0.08973f, 0.09012f, 0.09051f,
		0.09091f, 0.09130f, 0.09169f, 0.09208f,
		0.09247f, 0.09286f, 0.09324f, 0.09363f,
		0.09402f, 0.09440f, 0.09479f, 0.09518f,
		0.09556f, 0.09595f, 0.09633f, 0.09672f,
		0.09710f, 0.09749f, 0.09787f, 0.09825f,
		0.09863f, 0.09901f, 0.09939f, 0.09978f,
		0.10016f, 0.10054f, 0.10092f, 0.10130f,
		0.10167f, 0.10205f, 0.10243f, 0.10281f,
		0.10319f, 0.10356f, 0.10394f, 0.10432f,
		0.10469f, 0.10507f, 0.10544f, 0.10582f,
		0.10619f, 0.10657f, 0.10694f, 0.10731f,
		0.10768f, 0.10806f, 0.10843f, 0.10880f,
		0.10917f, 0.10954f, 0.10991f, 0.11029f,
		0.11066f, 0.11103f, 0.11141f, 0.11178f,
		0.11215f, 0.11253f, 0.11290f, 0.11328f,
		0.11365f, 0.11403f, 0.11440f, 0.11478f,
		0.11516f, 0.11553f, 0.11591f, 0.11629f,
		0.11666f, 0.11704f, 0.11742f, 0.11780f,
		0.11818f, 0.11856f, 0.11894f, 0.11932f,
		0.11970f, 0.12008f, 0.12046f, 0.12084f,
		0.12122f, 0.12161f, 0.12199f, 0.12237f,
		0.12276f, 0.12314f, 0.12352f, 0.12391f,
		0.12429f, 0.12468f, 0.12506f, 0.12545f,
		0.12583f, 0.12622f, 0.12661f, 0.12700f,
		0.12738f, 0.12777f, 0.12816f, 0.12855f,
		0.12894f, 0.12933f, 0.12972f, 0.13011f,
		0.13050f, 0.13089f, 0.13129f, 0.13168f,
		0.13207f, 0.13247f, 0.13286f, 0.13325f,
		0.13365f, 0.13404f, 0.13444f, 0.13483f,
		0.13523f, 0.13563f, 0.13603f, 0.13642f,
		0.13682f, 0.13722f, 0.13762f, 0.13802f,
		0.13842f, 0.13882f, 0.13922f, 0.13962f,
		0.14003f, 0.14043f, 0.14083f, 0.14124f,
		0.14164f, 0.14204f, 0.14245f, 0.14285f,
		0.14326f, 0.14366f, 0.14407f, 0.14448f,
		0.14489f, 0.14530f, 0.14570f, 0.14611f,
		0.14652f, 0.14693f, 0.14734f, 0.14776f,
		0.14817f, 0.14858f, 0.14900f, 0.14941f,
		0.14982f, 0.15024f, 0.15065f, 0.15107f,
		0.15148f, 0.15190f, 0.15232f, 0.15274f,
		0.15316f, 0.15357f, 0.15399f, 0.15441f,
		0.15483f, 0.15526f, 0.15568f, 0.15610f,
		0.15652f, 0.15695f, 0.15737f, 0.15779f,
		0.15822f, 0.15864f, 0.15907f, 0.15950f,
		0.15992f, 0.16035f, 0.16078f, 0.16121f,
		0.16164f, 0.16207f, 0.16250f, 0.16293f,
		0.16337f, 0.16380f, 0.16423f, 0.16467f,
		0.16511f, 0.16554f, 0.16598f, 0.16641f,
		0.16685f, 0.16729f, 0.16773f, 0.16816f,
		0.16860f, 0.16904f, 0.16949f, 0.16993f,
		0.17037f, 0.17081f, 0.17126f, 0.17170f,
		0.17215f, 0.17259f, 0.17304f, 0.17349f,
		0.17393f, 0.17438f, 0.17483f, 0.17528f,
		0.17573f, 0.17619f, 0.17664f, 0.17709f,
		0.17754f, 0.17799f, 0.17845f, 0.17890f,
		0.17936f, 0.17982f, 0.18028f, 0.18073f,
		0.18119f, 0.18165f, 0.18211f, 0.18257f,
		0.18303f, 0.18350f, 0.18396f, 0.18442f,
		0.18489f, 0.18535f, 0.18582f, 0.18629f,
		0.18676f, 0.18723f, 0.18770f, 0.18817f,
		0.18864f, 0.18911f, 0.18958f, 0.19005f,
		0.19053f, 0.19100f, 0.19147f, 0.19195f,
		0.19243f, 0.19291f, 0.19339f, 0.19387f,
		0.19435f, 0.19483f, 0.19531f, 0.19579f,
		0.19627f, 0.19676f, 0.19724f, 0.19773f,
		0.19821f, 0.19870f, 0.19919f, 0.19968f,
		0.20017f, 0.20066f, 0.20115f, 0.20164f,
		0.20214f, 0.20263f, 0.20313f, 0.20362f,
		0.20412f, 0.20462f, 0.20512f, 0.20561f,
		0.20611f, 0.20662f, 0.20712f, 0.20762f,
		0.20812f, 0.20863f, 0.20913f, 0.20964f,
		0.21015f, 0.21066f, 0.21117f, 0.21168f,
		0.21219f, 0.21270f, 0.21321f, 0.21373f,
		0.21424f, 0.21476f, 0.21527f, 0.21579f,
		0.21631f, 0.21683f, 0.21735f, 0.21787f,
		0.21839f, 0.21892f, 0.21944f, 0.21997f,
		0.22049f, 0.22102f, 0.22155f, 0.22208f,
		0.22261f, 0.22314f, 0.22367f, 0.22420f,
		0.22474f, 0.22527f, 0.22581f, 0.22634f,
		0.22688f, 0.22742f, 0.22796f, 0.22850f,
		0.22905f, 0.22959f, 0.23013f, 0.23068f,
		0.23123f, 0.23178f, 0.23232f, 0.23287f,
		0.23343f, 0.23398f, 0.23453f, 0.23508f,
		0.23564f, 0.23620f, 0.23675f, 0.23731f,
		0.23787f, 0.23843f, 0.23899f, 0.23956f,
		0.24012f, 0.24069f, 0.24125f, 0.24182f,
		0.24239f, 0.24296f, 0.24353f, 0.24410f,
		0.24468f, 0.24525f, 0.24582f, 0.24640f,
		0.24698f, 0.24756f, 0.24814f, 0.24872f,
		0.24931f, 0.24989f, 0.25048f, 0.25106f,
		0.25165f, 0.25224f, 0.25283f, 0.25342f,
		0.25401f, 0.25460f, 0.25520f, 0.25579f,
		0.25639f, 0.25699f, 0.25759f, 0.25820f,
		0.25880f, 0.25940f, 0.26001f, 0.26062f,
		0.26122f, 0.26183f, 0.26244f, 0.26306f,
		0.26367f, 0.26429f, 0.26490f, 0.26552f,
		0.26614f, 0.26676f, 0.26738f, 0.26800f,
		0.26863f, 0.26925f, 0.26988f, 0.27051f,
		0.27114f, 0.27177f, 0.27240f, 0.27303f,
		0.27367f, 0.27431f, 0.27495f, 0.27558f,
		0.27623f, 0.27687f, 0.27751f, 0.27816f,
		0.27881f, 0.27945f, 0.28011f, 0.28076f,
		0.28141f, 0.28207f, 0.28272f, 0.28338f,
		0.28404f, 0.28470f, 0.28536f, 0.28602f,
		0.28669f, 0.28736f, 0.28802f, 0.28869f,
		0.28937f, 0.29004f, 0.29071f, 0.29139f,
		0.29207f, 0.29274f, 0.29342f, 0.29410f,
		0.29479f, 0.29548f, 0.29616f, 0.29685f,
		0.29754f, 0.29823f, 0.29893f, 0.29962f,
		0.30032f, 0.30102f, 0.30172f, 0.30242f,
		0.30312f, 0.30383f, 0.30453f, 0.30524f,
		0.30595f, 0.30667f, 0.30738f, 0.30809f,
		0.30881f, 0.30953f, 0.31025f, 0.31097f,
		0.31170f, 0.31242f, 0.31315f, 0.31388f,
		0.31461f, 0.31534f, 0.31608f, 0.31682f,
		0.31755f, 0.31829f, 0.31904f, 0.31978f,
		0.32053f, 0.32127f, 0.32202f, 0.32277f,
		0.32353f, 0.32428f, 0.32504f, 0.32580f,
		0.32656f, 0.32732f, 0.32808f, 0.32885f,
		0.32962f, 0.33039f, 0.33116f, 0.33193f,
		0.33271f, 0.33349f, 0.33427f, 0.33505f,
		0.33583f, 0.33662f, 0.33741f, 0.33820f,
		0.33899f, 0.33978f, 0.34058f, 0.34138f,
		0.34218f, 0.34298f, 0.34378f, 0.34459f,
		0.34540f, 0.34621f, 0.34702f, 0.34783f,
		0.34865f, 0.34947f, 0.35029f, 0.35111f,
		0.35194f, 0.35277f, 0.35360f, 0.35443f,
		0.35526f, 0.35610f, 0.35694f, 0.35778f,
		0.35862f, 0.35946f, 0.36032f, 0.36117f,
		0.36202f, 0.36287f, 0.36372f, 0.36458f,
		0.36545f, 0.36631f, 0.36718f, 0.36805f,
		0.36891f, 0.36979f, 0.37066f, 0.37154f,
		0.37242f, 0.37331f, 0.37419f, 0.37507f,
		0.37596f, 0.37686f, 0.37775f, 0.37865f,
		0.37955f, 0.38045f, 0.38136f, 0.38227f,
		0.38317f, 0.38409f, 0.38500f, 0.38592f,
		0.38684f, 0.38776f, 0.38869f, 0.38961f,
		0.39055f, 0.39148f, 0.39242f, 0.39335f,
		0.39430f, 0.39524f, 0.39619f, 0.39714f,
		0.39809f, 0.39904f, 0.40000f, 0.40097f,
		0.40193f, 0.40289f, 0.40386f, 0.40483f,
		0.40581f, 0.40679f, 0.40777f, 0.40875f,
		0.40974f, 0.41073f, 0.41172f, 0.41272f,
		0.41372f, 0.41472f, 0.41572f, 0.41673f,
		0.41774f, 0.41875f, 0.41977f, 0.42079f,
		0.42181f, 0.42284f, 0.42386f, 0.42490f,
		0.42594f, 0.42697f, 0.42801f, 0.42906f,
		0.43011f, 0.43116f, 0.43222f, 0.43327f,
		0.43434f, 0.43540f, 0.43647f, 0.43754f,
		0.43862f, 0.43970f, 0.44077f, 0.44186f,
		0.44295f, 0.44404f, 0.44514f, 0.44624f,
		0.44734f, 0.44845f, 0.44956f, 0.45068f,
		0.45179f, 0.45291f, 0.45404f, 0.45516f,
		0.45630f, 0.45744f, 0.45858f, 0.45972f,
		0.46086f, 0.46202f, 0.46318f, 0.46433f,
		0.46550f, 0.46667f, 0.46784f, 0.46901f,
		0.47019f, 0.47137f, 0.47256f, 0.47375f,
		0.47495f, 0.47615f, 0.47735f, 0.47856f,
		0.47977f, 0.48099f, 0.48222f, 0.48344f,
		0.48467f, 0.48590f, 0.48714f, 0.48838f,
		0.48963f, 0.49088f, 0.49213f, 0.49340f,
		0.49466f, 0.49593f, 0.49721f, 0.49849f,
		0.49977f, 0.50106f, 0.50236f, 0.50366f,
		0.50496f, 0.50627f, 0.50758f, 0.50890f,
		0.51023f, 0.51155f, 0.51289f, 0.51422f,
		0.51556f, 0.51692f, 0.51827f, 0.51964f,
		0.52100f, 0.52237f, 0.52374f, 0.52512f,
		0.52651f, 0.52790f, 0.52930f, 0.53070f,
		0.53212f, 0.53353f, 0.53495f, 0.53638f,
		0.53781f, 0.53925f, 0.54070f, 0.54214f,
		0.54360f, 0.54506f, 0.54653f, 0.54800f,
		0.54949f, 0.55098f, 0.55247f, 0.55396f,
		0.55548f, 0.55699f, 0.55851f, 0.56003f,
		0.56156f, 0.56310f, 0.56464f, 0.56621f,
		0.56777f, 0.56933f, 0.57091f, 0.57248f,
		0.57407f, 0.57568f, 0.57727f, 0.57888f,
		0.58050f, 0.58213f, 0.58376f, 0.58541f,
		0.58705f, 0.58871f, 0.59037f, 0.59204f,
		0.59373f, 0.59541f, 0.59712f, 0.59882f,
		0.60052f, 0.60226f, 0.60399f, 0.60572f,
		0.60748f, 0.60922f, 0.61099f, 0.61276f,
		0.61455f, 0.61635f, 0.61814f, 0.61996f,
		0.62178f, 0.62361f, 0.62545f, 0.62730f,
		0.62917f, 0.63104f, 0.63291f, 0.63480f,
		0.63671f, 0.63862f, 0.64054f, 0.64249f,
		0.64443f, 0.64638f, 0.64835f, 0.65033f,
		0.65232f, 0.65433f, 0.65633f, 0.65836f,
		0.66041f, 0.66245f, 0.66452f, 0.66660f,
		0.66868f, 0.67078f, 0.67290f, 0.67503f,
		0.67717f, 0.67932f, 0.68151f, 0.68368f,
		0.68587f, 0.68809f, 0.69033f, 0.69257f,
		0.69482f, 0.69709f, 0.69939f, 0.70169f,
		0.70402f, 0.70634f, 0.70869f, 0.71107f,
		0.71346f, 0.71587f, 0.71829f, 0.72073f,
		0.72320f, 0.72567f, 0.72818f, 0.73069f,
		0.73323f, 0.73579f, 0.73838f, 0.74098f,
		0.74360f, 0.74622f, 0.74890f, 0.75159f,
		0.75429f, 0.75704f, 0.75979f, 0.76257f,
		0.76537f, 0.76821f, 0.77109f, 0.77396f,
		0.77688f, 0.77982f, 0.78278f, 0.78579f,
		0.78883f, 0.79187f, 0.79498f, 0.79809f,
		0.80127f, 0.80445f, 0.80767f, 0.81095f,
		0.81424f, 0.81757f, 0.82094f, 0.82438f,
		0.82782f, 0.83133f, 0.83488f, 0.83847f,
		0.84210f, 0.84577f, 0.84951f, 0.85328f,
		0.85713f, 0.86103f, 0.86499f, 0.86900f,
		0.87306f, 0.87720f, 0.88139f, 0.88566f,
		0.89000f, 0.89442f, 0.89891f, 0.90350f,
		0.90818f, 0.91295f, 0.91780f, 0.92272f,
		0.92780f, 0.93299f, 0.93828f, 0.94369f,
		0.94926f, 0.95493f, 0.96082f, 0.96684f,
		0.97305f, 0.97943f, 0.98605f, 0.99291f,
		1.00000f
		};
		
	const uint32 kTableSize = sizeof (kTable    ) /
							  sizeof (kTable [0]);
		
	real32 y = (real32) x * (real32) (kTableSize - 1);
	
	int32 index = Pin_int32 (0, (int32) y, kTableSize - 2);
	
	real32 fract = y - (real32) index;
	
	return kTable [index    ] * (1.0f - fract) +
		   kTable [index + 1] * (       fract);
	
	}

/*****************************************************************************/

const dng_1d_function & dng_tone_curve_acr3_default::Get ()
	{
	
	static dng_tone_curve_acr3_default static_dng_tone_curve_acr3_default;
	
	return static_dng_tone_curve_acr3_default;
	
	}

/*****************************************************************************/

class dng_render_task: public dng_filter_task
	{
	
	protected:
	
		const dng_negative &fNegative;
	
		const dng_render &fParams;
		
		dng_point fSrcOffset;
		
		dng_vector fCameraWhite;
		dng_matrix fCameraToRGB;
		
		AutoPtr<dng_hue_sat_map> fHueSatMap;
		
		dng_1d_table fExposureRamp;
		
		AutoPtr<dng_hue_sat_map> fLookTable;
		
		dng_1d_table fToneCurve;
		
		dng_matrix fRGBtoFinal;
		
		dng_1d_table fEncodeGamma;

		AutoPtr<dng_1d_table> fHueSatMapEncode;
		AutoPtr<dng_1d_table> fHueSatMapDecode;

		AutoPtr<dng_1d_table> fLookTableEncode;
		AutoPtr<dng_1d_table> fLookTableDecode;
	
		AutoPtr<dng_memory_block> fTempBuffer [kMaxMPThreads];
		
	public:
	
		dng_render_task (const dng_image &srcImage,
						 dng_image &dstImage,
						 const dng_negative &negative,
						 const dng_render &params,
						 const dng_point &srcOffset);
	
		virtual dng_rect SrcArea (const dng_rect &dstArea);
			
		virtual void Start (uint32 threadCount,
							const dng_point &tileSize,
							dng_memory_allocator *allocator,
							dng_abort_sniffer *sniffer);
							
		virtual void ProcessArea (uint32 threadIndex,
								  dng_pixel_buffer &srcBuffer,
								  dng_pixel_buffer &dstBuffer);
		
	};

/*****************************************************************************/

dng_render_task::dng_render_task (const dng_image &srcImage,
								  dng_image &dstImage,
								  const dng_negative &negative,
								  const dng_render &params,
								  const dng_point &srcOffset)
								  
	:	dng_filter_task (srcImage,
						 dstImage)
						 
	,	fNegative  (negative )
	,	fParams    (params   )
	,	fSrcOffset (srcOffset)
	
	,	fCameraWhite ()
	,	fCameraToRGB ()
	
	,	fHueSatMap ()
	
	,	fExposureRamp ()
	
	,	fLookTable ()
	
	,	fToneCurve ()
	
	,	fRGBtoFinal ()
	
	,	fEncodeGamma ()

	,	fHueSatMapEncode ()
	,	fHueSatMapDecode ()

	,	fLookTableEncode ()
	,	fLookTableDecode ()
	
	{
	
	fSrcPixelType = ttFloat;
	fDstPixelType = ttFloat;
	
	}
			
/*****************************************************************************/

dng_rect dng_render_task::SrcArea (const dng_rect &dstArea)
	{
	
	return dstArea + fSrcOffset;
	
	}
	
/*****************************************************************************/

void dng_render_task::Start (uint32 threadCount,
							 const dng_point &tileSize,
							 dng_memory_allocator *allocator,
							 dng_abort_sniffer *sniffer)
	{
	
	dng_filter_task::Start (threadCount,
							tileSize,
							allocator,
							sniffer);
							
	// Compute camera space to linear ProPhoto RGB parameters.
	
	dng_camera_profile_id profileID;	// Default profile ID.
		
	if (!fNegative.IsMonochrome ())
		{
		
		AutoPtr<dng_color_spec> spec (fNegative.MakeColorSpec (profileID));
		
		if (fParams.WhiteXY ().IsValid ())
			{
			
			spec->SetWhiteXY (fParams.WhiteXY ());
			
			}
							 
		else if (fNegative.HasCameraNeutral ())
			{
			
			spec->SetWhiteXY (spec->NeutralToXY (fNegative.CameraNeutral ()));
			
			}
			
		else if (fNegative.HasCameraWhiteXY ())
			{
			
			spec->SetWhiteXY (fNegative.CameraWhiteXY ());
			
			}
			
		else
			{
			
			spec->SetWhiteXY (D55_xy_coord ());
			
			}
			
		fCameraWhite = spec->CameraWhite ();
		
		fCameraToRGB = dng_space_ProPhoto::Get ().MatrixFromPCS () *
					   spec->CameraToPCS ();
					   
		// Find Hue/Sat table, if any.
		
		const dng_camera_profile *profile = fNegative.ProfileByID (profileID);
		
		if (profile)
			{
			
			fHueSatMap.Reset (profile->HueSatMapForWhite (spec->WhiteXY ()));
			
			if (profile->HasLookTable ())
				{
				
				fLookTable.Reset (new dng_hue_sat_map (profile->LookTable ()));
				
				}

			if (profile->HueSatMapEncoding () != encoding_Linear)
				{
					
				BuildHueSatMapEncodingTable (*allocator,
											 profile->HueSatMapEncoding (),
											 fHueSatMapEncode,
											 fHueSatMapDecode,
											 false);
					
				}
			
			if (profile->LookTableEncoding () != encoding_Linear)
				{
					
				BuildHueSatMapEncodingTable (*allocator,
											 profile->LookTableEncoding (),
											 fLookTableEncode,
											 fLookTableDecode,
											 false);
					
				}
			
			}
		
		}
		
	// Compute exposure/shadows ramp.

	real64 exposure = fParams.Exposure () +
					  fNegative.TotalBaselineExposure (profileID) -
					  (log (fNegative.Stage3Gain ()) / log (2.0));
	
		{
		//can commented out changed from Max_real64 (0.0, exposure) to exposure, see below
		real64 white = 1.0 / pow (2.0, exposure);
		
		real64 black = fParams.Shadows () *
					   fNegative.ShadowScale () *
					   fNegative.Stage3Gain () *
					   0.001;
					   
		black = Min_real64 (black, 0.99 * white);
		//black = 0.003; //can commented out changed from black = Min_real64 (black, 0.99 * white) to exposure, see left
		dng_function_exposure_ramp rampFunction (white,
												 black,
												 black);

		#if qDNGValidate
				
		if (gVerbose)
			{
											
				printf ("    exposure = %.4f\n",
						exposure);						
				printf ("    fParams.Shadows () = %.4f\n",
						fParams.Shadows ());						
				printf ("    fNegative.ShadowScale () = %.4f\n",
						fNegative.ShadowScale ());						
				printf ("    fNegative.Stage3Gain () = %.4f\n",
						fNegative.Stage3Gain ());						
				printf ("    white = %.4f\n",
						white);						
				printf ("    black = %.4f\n",
						black);						
				printf ("    fParams.Exposure () = %.4f\n",
						fParams.Exposure ());						
				printf ("    fNegative.TotalBaselineExposure (profileID) = %.4f\n",
						fNegative.TotalBaselineExposure (profileID));						
				printf ("    (log (fNegative.Stage3Gain ()) / log (2.0)) = %.4f\n",
						(log (fNegative.Stage3Gain ()) / log (2.0)));						


			}
					
		#endif
		fExposureRamp.Initialize (*allocator, rampFunction);

		}
		
	// Compute tone curve.
	
		{
		
		// If there is any negative exposure compenation to perform
		// (beyond what the camera provides for with its baseline exposure),
		// we fake this by darkening the tone curve.
		
		dng_function_exposure_tone exposureTone (exposure);
		
		dng_1d_concatenate totalTone (exposureTone,
									  fParams.ToneCurve ());
		
		fToneCurve.Initialize (*allocator, totalTone);
				
		}
		
	// Compute linear ProPhoto RGB to final space parameters.
	
		{
		
		const dng_color_space &finalSpace = fParams.FinalSpace ();
		
		fRGBtoFinal = finalSpace.MatrixFromPCS () *
					  dng_space_ProPhoto::Get ().MatrixToPCS ();
					  
		fEncodeGamma.Initialize (*allocator, finalSpace.GammaFunction ());
		
		}

	// Allocate temp buffer to hold one row of RGB data.
							
	uint32 tempBufferSize = tileSize.h * (uint32) sizeof (real32) * 3;
	
	for (uint32 threadIndex = 0; threadIndex < threadCount; threadIndex++)
		{
		
		fTempBuffer [threadIndex] . Reset (allocator->Allocate (tempBufferSize));
		
		}

	}
							
/*****************************************************************************/

void dng_render_task::ProcessArea (uint32 threadIndex,
								   dng_pixel_buffer &srcBuffer,
								   dng_pixel_buffer &dstBuffer)
	{
	
	dng_rect srcArea = srcBuffer.fArea;
	dng_rect dstArea = dstBuffer.fArea;
	
	uint32 srcCols = srcArea.W ();
	
	real32 *tPtrR = fTempBuffer [threadIndex]->Buffer_real32 ();
	
	real32 *tPtrG = tPtrR + srcCols;
	real32 *tPtrB = tPtrG + srcCols;

	std::fstream fs1;
	std::fstream fs2;
	std::fstream fs3;
	
	int stage_settings = 0;

	std::fstream stageSettingsFile(".\\stageSettings.txt", std::ios_base::in);

	real32 value;
	stageSettingsFile >> value;					
	stage_settings = real32(value);
	stageSettingsFile.close();


	int last_stage = 0;

	std::fstream lastStageFile(".\\lastStage.txt", std::ios_base::in);

	real32 lastStageValue;
	lastStageFile >> lastStageValue;					
	last_stage = real32(lastStageValue);
	lastStageFile.close();



	std::string save_directory = ".\\image";
	mkdir(save_directory.c_str());

	int rw_settings = 0;
		
	std::fstream rwSettingsFile(".\\rwSettings.txt", std::ios_base::in);

	real32 rwValue;
	rwSettingsFile >> rwValue;					
	rw_settings = real32(rwValue);



	rwSettingsFile.close();

	if(rw_settings == 0){
		fs1.open (save_directory + "\\r.txt", std::ios::out | std::ios::binary );
		fs2.open (save_directory + "\\g.txt", std::ios::out | std::ios::binary );
		fs3.open (save_directory + "\\b.txt", std::ios::out | std::ios::binary );	
	}

	if(rw_settings == 1){
		fs1.open (save_directory + "\\r.txt", std::ios::in | std::ios::binary);
		fs2.open (save_directory + "\\g.txt", std::ios::in | std::ios::binary);
		fs3.open (save_directory + "\\b.txt", std::ios::in | std::ios::binary);
	}

	std::cout<<"Current image size: "<<srcCols<<" x "<<srcArea.H ()<<"\n";

	//int stage_settings = 0;

	//std::fstream stageSettingsFile(".\\stageSettings.txt", std::ios_base::in);

	//real32 value;
	//stageSettingsFile >> value;					
	//stage_settings = real32(value);
	//stageSettingsFile.close();

				int cam_settings = 0 ;

					std::fstream cam_settingsfile(".\\cam_settings.txt", std::ios_base::in);

					real32 cam_settingsvalue;
					cam_settingsfile >> cam_settingsvalue;					
					cam_settings = real32(cam_settingsvalue);

					if(cam_settings == 1) {
						std::fstream myfile(".\\CAM.txt", std::ios_base::in);
						real32 value1;
						real32 value2;
						real32 value3;
						real32 value4;
						real32 value5;
						real32 value6;
						real32 value7;
						real32 value8;
						real32 value9;

						myfile >> value1;
						myfile >> value2;
						myfile >> value3;
						myfile >> value4;
						myfile >> value5;
						myfile >> value6;
						myfile >> value7;
						myfile >> value8;
						myfile >> value9;

						fCameraToRGB[0][0] = value1;
						fCameraToRGB[0][1] = value2;
						fCameraToRGB[0][2] = value3;
						fCameraToRGB[1][0] = value4;
						fCameraToRGB[1][1] = value5;
						fCameraToRGB[1][2] = value6;
						fCameraToRGB[2][0] = value7;
						fCameraToRGB[2][1] = value8;
						fCameraToRGB[2][2] = value9;
						myfile.close();
				}
					cam_settingsfile.close();

	int cam_printed = 0; 

	for (int32 srcRow = srcArea.t; srcRow < srcArea.b; srcRow++)
		{
		
		// First convert from camera native space to linear PhotoRGB,
		// applying the white balance and camera profile.
		
			{
			
			const real32 *sPtrA = (const real32 *)
								  srcBuffer.ConstPixel (srcRow,
													    srcArea.l,
													    0);
													   
			if (fSrcPlanes == 1)
				{
				
				// For monochrome cameras, this just requires copying
				// the data into all three color channels.
				
				DoCopyBytes (sPtrA, tPtrR, srcCols * (uint32) sizeof (real32));
				DoCopyBytes (sPtrA, tPtrG, srcCols * (uint32) sizeof (real32));
				DoCopyBytes (sPtrA, tPtrB, srcCols * (uint32) sizeof (real32));
				
				}
				
			else
				{
				
				const real32 *sPtrB = sPtrA + srcBuffer.fPlaneStep;
				const real32 *sPtrC = sPtrB + srcBuffer.fPlaneStep;


				if(cam_printed == 0 ){
					cam_printed = 1;
					printf ("    fCameraWhite = \n");
					int line_indices;
					for (int line_indices=0; line_indices<3; line_indices++)
					{
						printf("%.4f     ", fCameraWhite[line_indices]);
					}
					printf("\n");

					printf ("    fCameraToRGB = \n");
					int row_indices, columns_indices;
					for (int row_indices=0; row_indices<3; row_indices++)
					{
						for(int columns_indices=0; columns_indices<3; columns_indices++)
								printf("%.4f     ", fCameraToRGB[row_indices][columns_indices]);
						printf("\n");
					}
					printf("\n");
				}

				if(stage_settings == 4){
					// hakki commented out edited
					// This part writes Stage4 image to file. 
					if(rw_settings == 0){
						double* pDblA = (double*)malloc(srcCols * sizeof(double));
						double* pDblB = (double*)malloc(srcCols * sizeof(double));
						double* pDblC = (double*)malloc(srcCols * sizeof(double));

						for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
								pDblA[tPtrRindex] =  (double)sPtrA[tPtrRindex] ;																					
								pDblB[tPtrRindex] =  (double)sPtrB[tPtrRindex] ;																					
								pDblC[tPtrRindex] =  (double)sPtrC[tPtrRindex] ;																					
							
						}//end for 

						fs1.write((char*)pDblA, srcCols*sizeof(double));
						fs2.write((char*)pDblB, srcCols*sizeof(double));
						fs3.write((char*)pDblC, srcCols*sizeof(double));
					}//end if(rw_settings == 0)
				} // end if(stage_settings == 4) 
				
				//hakki can commented out added
				//fCameraToRGB.SetIdentity(3);
				if(stage_settings >= 5){
				if (fSrcPlanes == 3)
					{
					if(last_stage == 4){
						// hakki commented out edited
						// This part reads Stage4 image from file. 
						if(rw_settings == 1){
							double* pDblA = (double*)malloc(srcCols * sizeof(double));
							double* pDblB = (double*)malloc(srcCols * sizeof(double));
							double* pDblC = (double*)malloc(srcCols * sizeof(double));
							fs1.read((char*)pDblA, srcCols*sizeof(double));
							fs2.read((char*)pDblB, srcCols*sizeof(double));
							fs3.read((char*)pDblC, srcCols*sizeof(double));
								
							for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){

								tPtrR[tPtrRindex] = (real32)pDblA[tPtrRindex];																					
								tPtrG[tPtrRindex] = (real32)pDblB[tPtrRindex];																					
								tPtrB[tPtrRindex] = (real32)pDblC[tPtrRindex];																					
							}//end for 

							DoBaselineABCtoRGB (tPtrR,
												tPtrG,
												tPtrB,
												tPtrR,
												tPtrG,
												tPtrB,
												srcCols,
												fCameraWhite,
												fCameraToRGB);
						}//end if(rw_settings == 1)
					}//if(last_stage == 4)	
					else{
					DoBaselineABCtoRGB (sPtrA,
									    sPtrB,
									    sPtrC,
									    tPtrR,
									    tPtrG,
									    tPtrB,
									    srcCols,
									    fCameraWhite,
									    fCameraToRGB);
					}

					//#if qDNGValidate
				
					if (gVerbose)
						{
											
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[0][0]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[0][1]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[0][2]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[1][0]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[1][1]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[1][2]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[2][0]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[2][1]);						
							printf ("    fCameraToRGB = %.4f\n",
									fCameraToRGB[2][2]);						
												
						}
					
					//#endif
					
					}
					
				else
					{
					
					const real32 *sPtrD = sPtrC + srcBuffer.fPlaneStep;

					DoBaselineABCDtoRGB (sPtrA,
									     sPtrB,
									     sPtrC,
									     sPtrD,
									     tPtrR,
									     tPtrG,
									     tPtrB,
									     srcCols,
									     fCameraWhite,
									     fCameraToRGB);
					
					}
					
					if(stage_settings == 5){
						// hakki commented out edited
						// This part writes Stage5 image to file. 
						if(rw_settings == 0){
							double* pDblA = (double*)malloc(srcCols * sizeof(double));
							double* pDblB = (double*)malloc(srcCols * sizeof(double));
							double* pDblC = (double*)malloc(srcCols * sizeof(double));

							for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
									pDblA[tPtrRindex] =  (double)tPtrR[tPtrRindex] ;																					
									pDblB[tPtrRindex] =  (double)tPtrG[tPtrRindex] ;																					
									pDblC[tPtrRindex] =  (double)tPtrB[tPtrRindex] ;																					
							
							}//end for 

							fs1.write((char*)pDblA, srcCols*sizeof(double));
							fs2.write((char*)pDblB, srcCols*sizeof(double));
							fs3.write((char*)pDblC, srcCols*sizeof(double));

						}//end if(rw_settings == 0)
					} // end if(stage_settings == 5) 
				}//end if(stage_settings >= 5)	

				// Apply Hue/Sat map, if any.
				 //can commented out. 
				if(stage_settings >= 6)
					{
					if(last_stage == 5){
						// hakki commented out edited
						// This part reads Stage5 image from file. 
						if(rw_settings == 1){
	
							double* pDblA = (double*)malloc(srcCols * sizeof(double));
							double* pDblB = (double*)malloc(srcCols * sizeof(double));
							double* pDblC = (double*)malloc(srcCols * sizeof(double));
							fs1.read((char*)pDblA, srcCols*sizeof(double));
							fs2.read((char*)pDblB, srcCols*sizeof(double));
							fs3.read((char*)pDblC, srcCols*sizeof(double));
								
							for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){

								tPtrR[tPtrRindex] = (real32)pDblA[tPtrRindex];																					
								tPtrG[tPtrRindex] = (real32)pDblB[tPtrRindex];																					
								tPtrB[tPtrRindex] = (real32)pDblC[tPtrRindex];																					
							}//end for 

						}//end if(rw_settings == 1)
					}//if(last_stage == 5)

					if (fHueSatMap.Get ())
						{					
						DoBaselineHueSatMap (tPtrR,
											 tPtrG,
											 tPtrB,
											 tPtrR,
											 tPtrG,
											 tPtrB,
											 srcCols,
											 *fHueSatMap.Get (),
											 fHueSatMapEncode.Get (),
											 fHueSatMapDecode.Get ());
					
						} //end if (fHueSatMap.Get ())

						if(stage_settings == 6) {
							// hakki commented out edited
							// This part writes Stage6 image to file. 
							if(rw_settings == 0){
								double* pDblA = (double*)malloc(srcCols * sizeof(double));
								double* pDblB = (double*)malloc(srcCols * sizeof(double));
								double* pDblC = (double*)malloc(srcCols * sizeof(double));

								for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
										pDblA[tPtrRindex] =  (double)tPtrR[tPtrRindex] ;																					
										pDblB[tPtrRindex] =  (double)tPtrG[tPtrRindex] ;																					
										pDblC[tPtrRindex] =  (double)tPtrB[tPtrRindex] ;																					
							
								}//end for 

								fs1.write((char*)pDblA, srcCols*sizeof(double));
								fs2.write((char*)pDblB, srcCols*sizeof(double));
								fs3.write((char*)pDblC, srcCols*sizeof(double));

							}//end if(rw_settings == 0)
						}// end if(stage_settings == 6) 
					}// end if(stage_settings >= 6)
				}
				
			}
			
		// Apply exposure curve.
		// can commented out.
		if(stage_settings >= 7)
			{
			
			if(last_stage == 6){
				// hakki commented out edited
				// This part reads Stage6 image from file. 
				if(rw_settings == 1){					

					double* pDblA = (double*)malloc(srcCols * sizeof(double));
					double* pDblB = (double*)malloc(srcCols * sizeof(double));
					double* pDblC = (double*)malloc(srcCols * sizeof(double));
					fs1.read((char*)pDblA, srcCols*sizeof(double));
					fs2.read((char*)pDblB, srcCols*sizeof(double));
					fs3.read((char*)pDblC, srcCols*sizeof(double));
								
					for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){

						tPtrR[tPtrRindex] = (real32)pDblA[tPtrRindex];																					
						tPtrG[tPtrRindex] = (real32)pDblB[tPtrRindex];																					
						tPtrB[tPtrRindex] = (real32)pDblC[tPtrRindex];																					
					}//end for 

				}//end if(rw_settings == 1)
			}//if(last_stage == 6)

			DoBaseline1DTable (tPtrR,
							   tPtrR,
							   srcCols,
							   fExposureRamp);
								
			DoBaseline1DTable (tPtrG,
							   tPtrG,
							   srcCols,
							   fExposureRamp);
								
			DoBaseline1DTable (tPtrB,
							   tPtrB,
							   srcCols,
							   fExposureRamp);

			if(stage_settings == 7){
				// hakki commented out edited
				// This part writes Stage7 image to file. 
				if(rw_settings == 0){
					double* pDblA = (double*)malloc(srcCols * sizeof(double));
					double* pDblB = (double*)malloc(srcCols * sizeof(double));
					double* pDblC = (double*)malloc(srcCols * sizeof(double));

					for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
							pDblA[tPtrRindex] =  (double)tPtrR[tPtrRindex] ;																					
							pDblB[tPtrRindex] =  (double)tPtrG[tPtrRindex] ;																					
							pDblC[tPtrRindex] =  (double)tPtrB[tPtrRindex] ;																					
							
					}//end for 

					fs1.write((char*)pDblA, srcCols*sizeof(double));
					fs2.write((char*)pDblB, srcCols*sizeof(double));
					fs3.write((char*)pDblC, srcCols*sizeof(double));

				}//end if(rw_settings == 0)
			}// end if(stage_settings == 7) 
			} // end if(stage_settings >= 7)
	
		// Apply look table, if any.
		// can commented out. 
		if(stage_settings >= 8)
			{
			if(last_stage == 7){
				// hakki commented out edited
				// This part reads Stage7 image from file. 
				if(rw_settings == 1){
 
					double* pDblA = (double*)malloc(srcCols * sizeof(double));
					double* pDblB = (double*)malloc(srcCols * sizeof(double));
					double* pDblC = (double*)malloc(srcCols * sizeof(double));
					fs1.read((char*)pDblA, srcCols*sizeof(double));
					fs2.read((char*)pDblB, srcCols*sizeof(double));
					fs3.read((char*)pDblC, srcCols*sizeof(double));
								
					for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){

						tPtrR[tPtrRindex] = (real32)pDblA[tPtrRindex];																					
						tPtrG[tPtrRindex] = (real32)pDblB[tPtrRindex];																					
						tPtrB[tPtrRindex] = (real32)pDblC[tPtrRindex];																					
					}//end for 

				}//end if(rw_settings == 1)
			}//if(last_stage == 7)

			if (fLookTable.Get ())
				{		
				DoBaselineHueSatMap (tPtrR,
									 tPtrG,
									 tPtrB,
									 tPtrR,
									 tPtrG,
									 tPtrB,
									 srcCols,
									 *fLookTable.Get (),
									 fLookTableEncode.Get (),
									 fLookTableDecode.Get ());
			
				} // end if (fLookTable.Get ())

				if(stage_settings == 8){
					// hakki commented out edited
					// This part writes Stage8 image to file. 
					if(rw_settings == 0){
						double* pDblA = (double*)malloc(srcCols * sizeof(double));
						double* pDblB = (double*)malloc(srcCols * sizeof(double));
						double* pDblC = (double*)malloc(srcCols * sizeof(double));

						for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
								pDblA[tPtrRindex] =  (double)tPtrR[tPtrRindex] ;																					
								pDblB[tPtrRindex] =  (double)tPtrG[tPtrRindex] ;																					
								pDblC[tPtrRindex] =  (double)tPtrB[tPtrRindex] ;																					
							
						}//end for 

						fs1.write((char*)pDblA, srcCols*sizeof(double));
						fs2.write((char*)pDblB, srcCols*sizeof(double));
						fs3.write((char*)pDblC, srcCols*sizeof(double));

					}//end if(rw_settings == 0)
				}// end if(stage_settings == 8)
		} //end if(stage_settings >= 8)
  ////     
		// Apply baseline tone curve.
		// can commented out. 
		//
		if(stage_settings >= 9)
			{
			
			if(last_stage == 8){
				// hakki commented out edited
				// This part reads Stage8 image from file. 
				if(rw_settings == 1){

					double* pDblA = (double*)malloc(srcCols * sizeof(double));
					double* pDblB = (double*)malloc(srcCols * sizeof(double));
					double* pDblC = (double*)malloc(srcCols * sizeof(double));
					fs1.read((char*)pDblA, srcCols*sizeof(double));
					fs2.read((char*)pDblB, srcCols*sizeof(double));
					fs3.read((char*)pDblC, srcCols*sizeof(double));
								
					for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){

						tPtrR[tPtrRindex] = (real32)pDblA[tPtrRindex];																					
						tPtrG[tPtrRindex] = (real32)pDblB[tPtrRindex];																					
						tPtrB[tPtrRindex] = (real32)pDblC[tPtrRindex];																					
					}//end for 

				}//end if(rw_settings == 1)
			}//if(last_stage == 8)

			DoBaselineRGBTone (tPtrR,
							   tPtrG,
							   tPtrB,
							   tPtrR,
							   tPtrG,
							   tPtrB,
							   srcCols,
							   fToneCurve);

			if(stage_settings == 9){
				// hakki commented out edited
				// This part writes Stage9 image to file. 
				if(rw_settings == 0){
					double* pDblA = (double*)malloc(srcCols * sizeof(double));
					double* pDblB = (double*)malloc(srcCols * sizeof(double));
					double* pDblC = (double*)malloc(srcCols * sizeof(double));

					for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
							pDblA[tPtrRindex] =  (double)tPtrR[tPtrRindex] ;																					
							pDblB[tPtrRindex] =  (double)tPtrG[tPtrRindex] ;																					
							pDblC[tPtrRindex] =  (double)tPtrB[tPtrRindex] ;																					
							
					}//end for 

					fs1.write((char*)pDblA, srcCols*sizeof(double));
					fs2.write((char*)pDblB, srcCols*sizeof(double));
					fs3.write((char*)pDblC, srcCols*sizeof(double));

				}//end if(rw_settings == 0)
			}//end if(stage_settings == 9)

			// Convert to final color space.
			if(stage_settings >= 10)
				{
				int32 dstRow = srcRow + (dstArea.t - srcArea.t);
		
				if (fDstPlanes == 1)
					{
			
					real32 *dPtrG = dstBuffer.DirtyPixel_real32 (dstRow,
																 dstArea.l,
																 0);

					DoBaselineRGBtoGray (tPtrR,
										 tPtrG,
										 tPtrB,
										 dPtrG,
										 srcCols,
										 fRGBtoFinal);
			
					DoBaseline1DTable (dPtrG,
									   dPtrG,
									   srcCols,
									   fEncodeGamma);
								
					}
		
				else
					{
			
					real32 *dPtrR = dstBuffer.DirtyPixel_real32 (dstRow,
																 dstArea.l,
																 0);
		
					real32 *dPtrG = dPtrR + dstBuffer.fPlaneStep;
					real32 *dPtrB = dPtrG + dstBuffer.fPlaneStep;
					if(last_stage == 9){
						// hakki commented out edited
						// This part reads Stage9 image from file. 
						if(rw_settings == 1){

							double* pDblA = (double*)malloc(srcCols * sizeof(double));
							double* pDblB = (double*)malloc(srcCols * sizeof(double));
							double* pDblC = (double*)malloc(srcCols * sizeof(double));
							fs1.read((char*)pDblA, srcCols*sizeof(double));
							fs2.read((char*)pDblB, srcCols*sizeof(double));
							fs3.read((char*)pDblC, srcCols*sizeof(double));
								
							for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){

								dPtrR[tPtrRindex] = (real32)pDblA[tPtrRindex];																					
								dPtrG[tPtrRindex] = (real32)pDblB[tPtrRindex];																					
								dPtrB[tPtrRindex] = (real32)pDblC[tPtrRindex];																					
							}//end for 

						}//end if(rw_settings == 1)
					}//if(last_stage == 9)

					//can commented out , Hakki: removed set Identity
					//fRGBtoFinal.SetIdentity(3);
					DoBaselineRGBtoRGB (tPtrR,
										tPtrG,
										tPtrB,
										dPtrR,
										dPtrG,
										dPtrB,
										srcCols,
										fRGBtoFinal);
					//can commented out ,	

					if(stage_settings == 10){
						// hakki commented out edited
						// This part writes Stage10 image to file. 
						if(rw_settings == 0){

							double* pDblA = (double*)malloc(srcCols * sizeof(double));
							double* pDblB = (double*)malloc(srcCols * sizeof(double));
							double* pDblC = (double*)malloc(srcCols * sizeof(double));

							for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
									pDblA[tPtrRindex] =  (double)dPtrR[tPtrRindex] ;																					
									pDblB[tPtrRindex] =  (double)dPtrG[tPtrRindex] ;																					
									pDblC[tPtrRindex] =  (double)dPtrB[tPtrRindex] ;																					
							
							}//end for 

							fs1.write((char*)pDblA, srcCols*sizeof(double));
							fs2.write((char*)pDblB, srcCols*sizeof(double));
							fs3.write((char*)pDblC, srcCols*sizeof(double));

						}//end if(rw_settings == 0)
					}//end if(stage_settings == 10)

					if(stage_settings >= 11)
						{
						if(last_stage == 10){
							// hakki commented out edited
							// This part reads Stage10 image from file. 
							if(rw_settings == 1){

								double* pDblA = (double*)malloc(srcCols * sizeof(double));
								double* pDblB = (double*)malloc(srcCols * sizeof(double));
								double* pDblC = (double*)malloc(srcCols * sizeof(double));
								fs1.read((char*)pDblA, srcCols*sizeof(double));
								fs2.read((char*)pDblB, srcCols*sizeof(double));
								fs3.read((char*)pDblC, srcCols*sizeof(double));
								
								for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){

									dPtrR[tPtrRindex] = (real32)pDblA[tPtrRindex];																					
									dPtrG[tPtrRindex] = (real32)pDblB[tPtrRindex];																					
									dPtrB[tPtrRindex] = (real32)pDblC[tPtrRindex];																					
								}//end for 

							}//end if(rw_settings == 1)
						}//if(last_stage == 10)

						DoBaseline1DTable (dPtrR,
											dPtrR,
											srcCols,
											fEncodeGamma);
								
						DoBaseline1DTable (dPtrG,
											dPtrG,
											srcCols,
											fEncodeGamma);
								
						DoBaseline1DTable (dPtrB,
											dPtrB,
											srcCols,
											fEncodeGamma);

						if(stage_settings == 11){
							// hakki commented out edited
							// This part writes Stage11 image to file. 
							if(rw_settings == 0){
								double* pDblA = (double*)malloc(srcCols * sizeof(double));
								double* pDblB = (double*)malloc(srcCols * sizeof(double));
								double* pDblC = (double*)malloc(srcCols * sizeof(double));

								for(int tPtrRindex = 0; tPtrRindex<srcCols;++tPtrRindex){
										pDblA[tPtrRindex] =  (double)dPtrR[tPtrRindex] ;																					
										pDblB[tPtrRindex] =  (double)dPtrG[tPtrRindex] ;																					
										pDblC[tPtrRindex] =  (double)dPtrB[tPtrRindex] ;																					
							
								}//end for 

								fs1.write((char*)pDblA, srcCols*sizeof(double));
								fs2.write((char*)pDblB, srcCols*sizeof(double));
								fs3.write((char*)pDblC, srcCols*sizeof(double));
							}//end if(rw_settings == 0)
						}// end if(stage_settings == 11)
					
						}//end if(stage_settings >= 11)		


					}
				}//end if(stage_settings >= 10)

				else
					{

					int32 dstRow = srcRow + (dstArea.t - srcArea.t);
		
					if (fDstPlanes == 1)
						{
			
						real32 *dPtrG = dstBuffer.DirtyPixel_real32 (dstRow,
																	 dstArea.l,
																	 0);

						DoBaselineRGBtoGray (tPtrR,
											 tPtrG,
											 tPtrB,
											 dPtrG,
											 srcCols,
											 fRGBtoFinal);
			
						DoBaseline1DTable (dPtrG,
										   dPtrG,
										   srcCols,
										   fEncodeGamma);
								
						}
		
					else
						{
			
						real32 *dPtrR = dstBuffer.DirtyPixel_real32 (dstRow,
																	 dstArea.l,
																	 0);
		
						real32 *dPtrG = dPtrR + dstBuffer.fPlaneStep;
						real32 *dPtrB = dPtrG + dstBuffer.fPlaneStep;
						//Rang:added set Identity
						//can commented out , Hakki: removed set Identity
						fRGBtoFinal.SetIdentity(3);
						DoBaselineRGBtoRGB (tPtrR,
											tPtrG,
											tPtrB,
											dPtrR,
											dPtrG,
											dPtrB,
											srcCols,
											fRGBtoFinal);
						//can commented out ,	
						//if(stage_settings >= 11){
						//DoBaseline1DTable (dPtrR,
						//				   dPtrR,
						//				   srcCols,
						//				   fEncodeGamma);
						//					
						//DoBaseline1DTable (dPtrG,
						//				   dPtrG,
						//				   srcCols,
						//				   fEncodeGamma);
						//					
						//DoBaseline1DTable (dPtrB,
						//				   dPtrB,
						//				   srcCols,
						//				   fEncodeGamma);

						//			}//end if(stage_settings >= 11)				   
						} // end else
		
		
					} // end else
			} //end if(stage_settings >= 9)

			else
				{

				int32 dstRow = srcRow + (dstArea.t - srcArea.t);
		
				if (fDstPlanes == 1)
					{
			
					real32 *dPtrG = dstBuffer.DirtyPixel_real32 (dstRow,
																 dstArea.l,
																 0);

					DoBaselineRGBtoGray (tPtrR,
										 tPtrG,
										 tPtrB,
										 dPtrG,
										 srcCols,
										 fRGBtoFinal);
			
					DoBaseline1DTable (dPtrG,
									   dPtrG,
									   srcCols,
									   fEncodeGamma);
								
					}
		
				else
					{
			
					real32 *dPtrR = dstBuffer.DirtyPixel_real32 (dstRow,
																 dstArea.l,
																 0);
		
					real32 *dPtrG = dPtrR + dstBuffer.fPlaneStep;
					real32 *dPtrB = dPtrG + dstBuffer.fPlaneStep;
					//Rang:added set Identity
					//can commented out , Hakki: removed set Identity
					fRGBtoFinal.SetIdentity(3);
					DoBaselineRGBtoRGB (tPtrR,
										tPtrG,
										tPtrB,
										dPtrR,
										dPtrG,
										dPtrB,
										srcCols,
										fRGBtoFinal);
					//can commented out ,	
					//if(stage_settings >= 11){
					//DoBaseline1DTable (dPtrR,
					//				   dPtrR,
					//				   srcCols,
					//				   fEncodeGamma);
					//					
					//DoBaseline1DTable (dPtrG,
					//				   dPtrG,
					//				   srcCols,
					//				   fEncodeGamma);
					//					
					//DoBaseline1DTable (dPtrB,
					//				   dPtrB,
					//				   srcCols,
					//				   fEncodeGamma);

					//			}//end if(stage_settings >= 11)				   
					} // end else
		
		
				} // end else
		} //end for (int32 srcRow = srcArea.t; srcRow < srcArea.b; srcRow++)
		fs1.close();
		fs2.close();
		fs3.close();
	}
		
/*****************************************************************************/
//can commented out changed fExposure(1.0) from fExposure(0.0) and fShadows(5,0) to fShadows(0,0)
dng_render::dng_render (dng_host &host,
						const dng_negative &negative)

	:	fHost			(host)
	,	fNegative		(negative)
	
	,	fWhiteXY		()
	
	,	fExposure		(0.0)
	,	fShadows		(5.0)
	
	,	fToneCurve		(&dng_tone_curve_acr3_default::Get ())
	
	,	fFinalSpace		(&dng_space_sRGB::Get ())
	,	fFinalPixelType (ttByte)
	
	,	fMaximumSize	(0)
	
	,	fProfileToneCurve ()
	
	{
	
	// Switch to NOP default parameters for non-scene referred data.
	std::fstream myfile(".\\EV.txt", std::ios_base::in);

	real32 value;
	myfile >> value;

	fExposure = value;
	
	if (fNegative.ColorimetricReference () != crSceneReferred)
		{
		
		fShadows = 0.0;
		
		fToneCurve = &dng_1d_identity::Get ();
		
		}
		
	// Use default tone curve from profile if any.
	
	const dng_camera_profile *profile = fNegative.ProfileByID (dng_camera_profile_id ());
	
	if (profile && profile->ToneCurve ().IsValid ())
		{
		
		fProfileToneCurve.Reset (new dng_spline_solver);
		
		profile->ToneCurve ().Solve (*fProfileToneCurve.Get ());
		
		fToneCurve = fProfileToneCurve.Get ();
		
		}

	// Turn off default shadow mapping if requested by profile.

	if (profile && (profile->DefaultBlackRender () == defaultBlackRender_None))
		{
		
		fShadows = 0.0;
		
		}
	
	}

/*****************************************************************************/

dng_image * dng_render::Render ()
	{
	
	const dng_image *srcImage = fNegative.Stage3Image ();
	
	dng_rect srcBounds = fNegative.DefaultCropArea ();
	
	dng_point dstSize;
	
	dstSize.h =	fNegative.DefaultFinalWidth  ();
	dstSize.v = fNegative.DefaultFinalHeight ();
								   
	if (MaximumSize ())
		{
		
		if (Max_uint32 (dstSize.h, dstSize.v) > MaximumSize ())
			{
			
			real64 ratio = fNegative.AspectRatio ();
			
			if (ratio >= 1.0)
				{
				dstSize.h = MaximumSize ();
				dstSize.v = Max_uint32 (1, Round_uint32 (dstSize.h / ratio));
				}
				
			else
				{
				dstSize.v = MaximumSize ();
				dstSize.h = Max_uint32 (1, Round_uint32 (dstSize.v * ratio));
				}
			
			}
		
		}
		
	AutoPtr<dng_image> tempImage;
	
	if (srcBounds.Size () != dstSize)
		{

		tempImage.Reset (fHost.Make_dng_image (dstSize,
											   srcImage->Planes    (),
											   srcImage->PixelType ()));
											 
		ResampleImage (fHost,
					   *srcImage,
					   *tempImage.Get (),
					   srcBounds,
					   tempImage->Bounds (),
					   dng_resample_bicubic::Get ());
						   
		srcImage = tempImage.Get ();
		
		srcBounds = tempImage->Bounds ();
		
		}
	
	uint32 dstPlanes = FinalSpace ().IsMonochrome () ? 1 : 3;
	
	AutoPtr<dng_image> dstImage (fHost.Make_dng_image (srcBounds.Size (),
													   dstPlanes,
													   FinalPixelType ()));
													 
	dng_render_task task (*srcImage,
						  *dstImage.Get (),
						  fNegative,
						  *this,
						  srcBounds.TL ());
						  
	fHost.PerformAreaTask (task,
						   dstImage->Bounds ());
						  
	return dstImage.Release ();
	
	}

/*****************************************************************************/