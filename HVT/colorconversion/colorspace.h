/*
 * colorspace.h
 *
 *  Created on: Aug 11, 2012
 *      Author: bogdan
 */

#ifndef COLORSPACES_H_
#define COLORSPACES_H_

#include<stdbool.h>
#include<stdint.h>
#include<stdlib.h>
#include<math.h>

/*The lower limit for R, G, B (real version), S, I*/
#define PER_LOWER_LIMIT (0.0)
/*The upper limit for R, G, B (real version), S, I*/
#define PER_UPPER_LIMIT (1.0)
/*The lower limit for H*/
#define HUE_LOWER_LIMIT (0.0)
/*The upper limit for H*/
#define HUE_UPPER_LIMIT (360.0)
/*The lower limit for R, G, B (integer version)*/
#define RGBI_LOWER_LIMIT (0U)
/*The upper limit for R, G, B (integer version)*/
#define RGBI_UPPER_LIMIT (255U)
/*The upper limit for I in YIQ*/
#define YIQ_I_UPPER_LIMIT (0.5957)
/*The lower limit for I in YIQ*/
#define YIQ_I_LOWER_LIMIT (-0.5957)
/*The upper limit for Q in YIQ*/
#define YIQ_Q_UPPER_LIMIT (0.5226)
/*The lower limit for Q in YIQ*/
#define YIQ_Q_LOWER_LIMIT (-0.5226)
/*The upper limit for U in YUV*/
#define YUV_U_UPPER_LIMIT (0.436)
/*The lower limit for U in YUV*/
#define YUV_U_LOWER_LIMIT (-0.436)
/*The upper limit for V in YUV*/
#define YUV_V_UPPER_LIMIT (0.615)
/*The lower limit for V in YUV*/
#define YUV_V_LOWER_LIMIT (-0.615)

typedef struct
{
    float R;
	float G;
	float B;
} RgbFColor;

typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RgbIColor;

typedef struct
{
    float H;
    float S;
    float I;
} HsiColor;

typedef struct
{
    float H;
    float S;
    float L;
} HslColor;

typedef struct
{
    float H;
    float S;
    float V;
} HsvColor;

typedef struct
{
    float Y;
    float I;
    float Q;
} YiqColor;

typedef struct
{
    float Y;
    float U;
    float V;
} YuvColor;

/*
 * Description:
 *  Checks if a value is within a specified interval
 * Parameters:
 *  value - the value who is checked
 *  lowerLimit - the lower limit of the interval
 *  upperLimit - the upper limit of the interval
 * Returns:
 *  true if the value is within the interval
 *  false if the value is not within the interval
 */bool RealIsWithinBounds(float value, float lowerLimit, float upperLimit);
/*
 * Description:
 *  Checks if a value is within a specified interval
 * Parameters:
 *  value - the value who is checked
 *  lowerLimit - the lower limit of the interval
 *  upperLimit - the upper limit of the interval
 * Returns:
 *  true if the value is within the interval
 *  false if the value is not within the interval
 */bool IntegerIsWithinBounds(uint8_t value, uint8_t lowerLimit,
        uint8_t upperLimit);
/*
 * Description:
 *  Returns the smallest of the three parameters
 * Parameters
 *  r,g,b - 3 real numbers
 * Returns
 *  The smallest real number from the set {r,g,b}
 */
float float_GetMinimum(float r, float g, float b);
/*
 * Description:
 *  Returns the largest of the three parameters
 * Parameters
 *  r,g,b - 3 real numbers
 * Returns
 *  The largest real number from the set {r,g,b}
 */
float float_GetMaximum(float r, float g, float b);

/*
 * Description:
 *  Checks if the RGB components are valid
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  true if the values are correct
 *  false if the values are incorrect
 */bool RgbF_IsValid(float r, float g, float b);
/*
 * Description:
 *  Checks if the HSI components are valid
 * Parameters:
 *  h,s,i - the components of an HSI model expressed
 *          as real numbers
 * Returns:
 *  true if the values are correct
 *  false if the values are incorrect
 */bool Hsi_IsValid(float h, float s, float i);
/*
 * Description:
 *  Checks if the RGB components are valid
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as natural numbers
 * Returns:
 *  true if the values are correct
 *  false if the values are incorrect
 */bool RgbI_IsValid(uint8_t r, uint8_t g, uint8_t b);
/*
 * Description:
 *  Checks if the HSL components are valid
 * Parameters:
 *  h,s,l - the components of an HSL model expressed
 *          as real numbers
 * Returns:
 *  true if the values are correct
 *  false if the values are incorrect
 */bool Hsl_IsValid(float h, float s, float l);
/*
 * Description:
 *  Checks if the HSV components are valid
 * Parameters:
 *  h,s,v - the components of an HSV model expressed
 *          as real numbers
 * Returns:
 *  true if the values are correct
 *  false if the values are incorrect
 */
//bool Hsv_IsValid(float h, float s, float v);
/*
 * Description:
 *  Checks if the YIQ components are valid
 * Parameters:
 *  y,i,q - the components of an YIQ model expressed
 *          as real numbers
 * Returns:
 *  true if the values are correct
 *  false if the values are incorrect
 */bool Yiq_IsValid(float y, float i, float q);
/*
 * Description:
 *  Checks if the YUV components are valid
 * Parameters:
 *  y,u,v - the components of an YUV model expressed
 *          as real numbers
 * Returns:
 *  true if the values are correct
 *  false if the values are incorrect
 */bool Yuv_IsValid(float y, float u, float v);

/*
 * Description:
 *  Creates a RgbIColor structure
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as natural numbers
 * Returns:
 *  A pointer to the RgbIColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbIColor* RgbI_Create(uint8_t r, uint8_t g, uint8_t b);
/*
 * Description:
 *  Creates a RgbIColor structure from RGB components
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbIColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbIColor* RgbI_CreateFromRealForm(float r, float g, float b);
/*
 * Description:
 *  Creates a RgbfColor structure
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbFColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbFColor* RgbF_Create(float r, float g, float b);
/*
 * Description:
 *  Creates a RgbFColor structure from RGB components
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbFColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbFColor* RgbF_CreateFromIntegerForm(uint8_t r, uint8_t g, uint8_t b);
/*
 * Description:
 *  Creates a RgbFColor structure from HSI components
 * Parameters:
 *  h,s,i - the components of an HSI model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbFColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbFColor* RgbF_CreateFromHsi(float h, float s, float i);
/*
 * Description:
 *  Creates a RgbFColor structure from HSL components
 * Parameters:
 *  h,s,l - the components of an HSL model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbFColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbFColor* RgbF_CreateFromHsl(float h, float s, float l);
/*
 * Description:
 *  Creates a RgbFColor structure from HSV components
 * Parameters:
 *  h,s,v - the components of an HSV model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbFColor is the parameters are
 * correct. Otherwise returns NULL.
 */
// RgbFColor* RgbF_CreateFromHsv(float h, float s, float v);
/*
 * Description:
 *  Creates a RgbFColor structure from YIQ components
 * Parameters:
 *  y,i,q - the components of an YIQ model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbFColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbFColor* RgbF_CreateFromYiq(float y, float i, float q);
/*
 * Description:
 *  Creates a RgbFColor structure from YUV components
 * Parameters:
 *  y,u,v - the components of an YUV model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the RgbFColor is the parameters are
 * correct. Otherwise returns NULL.
 */
RgbFColor* RgbF_CreateFromYuv(float y, float u, float v);
/*
 * Description:
 *  Creates a HsiColor structure
 * Parameters:
 *  h,s,i - the components of an HSI model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the HsiColor is the parameters are
 * correct. Otherwise returns NULL.
 */
HsiColor* Hsi_Create(float h, float s, float i);
/* Description:
 *  Creates a HsiColor structure from RGB components
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the HsiColor is the parameters are
 * correct. Otherwise returns NULL.
 */
HsiColor* Hsi_CreateFromRgbF(float r, float g, float b);
/*
 * Description:
 *  Creates a HslColor structure
 * Parameters:
 *  h,s,l - the components of an HSL model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the HslColor is the parameters are
 * correct. Otherwise returns NULL.
 */
HslColor* Hsl_Create(float h, float s, float l);
/* Description:
 *  Creates a HslColor structure from RGB components
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the HslColor is the parameters are
 * correct. Otherwise returns NULL.
 */
HslColor* Hsl_CreateFromRgbF(float r, float g, float b);
/*
 * Description:
 *  Creates a HsvColor structure
 * Parameters:
 *  h,s,v - the components of an HSV model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the HsvColor is the parameters are
 * correct. Otherwise returns NULL.
 */
HsvColor* Hsv_Create(float h, float s, float v);
/* Description:
 *  Creates a HsvColor structure from RGB components
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the HsvColor is the parameters are
 * correct. Otherwise returns NULL.
 */
HsvColor* Hsv_CreateFromRgbF(float r, float g, float b);
/*
 * Description:
 *  Creates a YiqColor structure
 * Parameters:
 *  y,i,q - the components of an YIQ model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the YiqColor is the parameters are
 * correct. Otherwise returns NULL.
 */
YiqColor* Yiq_Create(float y, float i, float q);
/* Description:
 *  Creates a YiqColor structure from RGB components
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the YiqColor is the parameters are
 * correct. Otherwise returns NULL.
 */
YiqColor* Yiq_CreateFromRgbF(float r, float g, float b);
/*
 * Description:
 *  Creates a YuvColor structure
 * Parameters:
 *  y,u,v - the components of an YUV model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the YuvColor is the parameters are
 * correct. Otherwise returns NULL.
 */
YuvColor* Yuv_Create(float y, float u, float v);
/* Description:
 *  Creates a YuvColor structure from RGB components
 * Parameters:
 *  r,g,b - the components of an RGB model expressed
 *          as real numbers
 * Returns:
 *  A pointer to the YuvColor is the parameters are
 * correct. Otherwise returns NULL.
 */
YuvColor* Yuv_CreateFromRgbF(float r, float g, float f);

bool RgbF_IsValid(float r, float g, float b)
{
	bool isValid = true;
	if ((RealIsWithinBounds(r, PER_LOWER_LIMIT, PER_UPPER_LIMIT) == false)
		|| (RealIsWithinBounds(g, PER_LOWER_LIMIT, PER_UPPER_LIMIT) == false)
		|| (RealIsWithinBounds(b, PER_LOWER_LIMIT, PER_UPPER_LIMIT) == false))
	{
		isValid = false;
	}
	return isValid;
}


RgbFColor* RgbF_Create(float r, float g, float b)
{
	RgbFColor* color = NULL;
	if (RgbF_IsValid(r, g, b) == true)
	{
		color = (RgbFColor*)malloc(sizeof(RgbFColor));
		if (color != NULL)
		{
			color->R = r;
			color->G = g;
			color->B = b;
		}
	}
	return color;
}

bool RealIsWithinBounds(float value, float lowerLimit, float upperLimit)
{
	if (value >= lowerLimit && value <= upperLimit)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Hsv_IsValid(float h, float s, float v)
{
	bool isValid = true;
	if ((RealIsWithinBounds(h, HUE_LOWER_LIMIT, HUE_UPPER_LIMIT) == false)
		|| (RealIsWithinBounds(s, PER_LOWER_LIMIT, PER_UPPER_LIMIT) == false)
		|| (RealIsWithinBounds(v, PER_LOWER_LIMIT, PER_UPPER_LIMIT) == false))
	{
		isValid = false;
	}
	return isValid;
}


HsvColor* Hsv_Create(float h, float s, float v)
{
	HsvColor* color = NULL;
	if (Hsv_IsValid(h, s, v) == true)
	{
		color = (HsvColor*)malloc(sizeof(HsvColor));
		if (color != NULL)
		{
			color->H = h;
			color->S = s;
			color->V = v;
		}
	}
	return color;
}


RgbFColor* RgbF_CreateFromHsv(float h, float s, float v)
{
	float c = 0.0, m = 0.0, x = 0.0;
	RgbFColor *color = NULL;
	if (Hsv_IsValid(h, s, v) == true)
	{
		c = v * s;
		x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2) - 1.0f));
		m = v - c;
		if (h >= 0.0 && h < 60.0)
		{
			color = RgbF_Create(c + m, x + m, m);
		}
		else if (h >= 60.0 && h < 120.0)
		{
			color = RgbF_Create(x + m, c + m, m);
		}
		else if (h >= 120.0 && h < 180.0)
		{
			color = RgbF_Create(m, c + m, x + m);
		}
		else if (h >= 180.0 && h < 240.0)
		{
			color = RgbF_Create(m, x + m, c + m);
		}
		else if (h >= 240.0 && h < 300.0)
		{
			color = RgbF_Create(x + m, m, c + m);
		}
		else if (h >= 300.0 && h < 360.0)
		{
			color = RgbF_Create(c + m, m, x + m);
		}
		else
		{
			color = RgbF_Create(m, m, m);
		}
	}
	return color;
}


#endif
