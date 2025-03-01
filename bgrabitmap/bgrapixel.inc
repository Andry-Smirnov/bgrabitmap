// SPDX-License-Identifier: LGPL-3.0-linking-exception
{=== Pixel types and functions ===}

{$IFDEF INCLUDE_INTERFACE}
{$UNDEF INCLUDE_INTERFACE}
type
  {* Pointer for direct pixel access. Data is stored as a sequence of TBGRAPixel.
     See [Tutorial 4](https://wiki.freepascal.org/BGRABitmap_tutorial_4) }
  PBGRAPixel = ^TBGRAPixel;

  {$IFNDEF BGRABITMAP_BGRAPIXEL}
    {$IFDEF BGRABITMAP_USE_LCL}
      {$IFDEF LCLgtk}
        {$DEFINE BGRABITMAP_RGBAPIXEL}
      {$ENDIF}
      {$IFDEF LCLgtk2}
        {$DEFINE BGRABITMAP_RGBAPIXEL}
      {$ENDIF}
      {$IFDEF DARWIN}
		{$IFNDEF LCLQt}
			{$DEFINE BGRABITMAP_RGBAPIXEL}
		{$ENDIF}
      {$ENDIF}
    {$ENDIF}
  {$ENDIF}

  {* Each pixel is a sequence of 4 bytes containing _blue_, _green_, _red_ and _alpha_ channel.
     Values range from 0 to 255, color is in sRGB colorspace. The _alpha_ value of 0
     is transparent and 255 is opaque. In the bitmap data, when the pixel is fully transparent,
	 the RGB values are supposed to be set to zero. }
  TBGRAPixel = packed record
  private
    { Calls function to compute intensity }
    function GetClassIntensity: word;
    { Calls function to compute lightness }
    function GetClassLightness: word;
    { Calls function to set intensity }
    procedure SetClassIntensity(AValue: word);
    { Calls function to set lightness }
    procedure SetClassLightness(AValue: word);
  public
    {$IFDEF BGRABITMAP_RGBAPIXEL}
    {** Channels if ordered RGBA ordered }
    red, green, blue, alpha: byte;
    {$ELSE}
    {** Channels if ordered BGRA ordered }
    blue, green, red, alpha: byte;
    {$ENDIF}
    class function New(const ARed,AGreen,ABlue,AAlpha:byte): TBGRAPixel;overload;static;
    class function New(const ARed,AGreen,ABlue:byte): TBGRAPixel;overload;static;
    {** Sets the pixel with given RGB values }
    procedure FromRGB(ARed,AGreen,ABlue: Byte; AAlpha: Byte = 255);
    {** Sets the pixel with given TColor }
    procedure FromColor(AColor: TColor; AAlpha: Byte = 255);
    {** Sets the pixel according to a color name or CSS expression }
    procedure FromString(AStr: string);
    {** Sets the pixel with given TFPColor  (16-bit per channel) }
    procedure FromFPColor(AColor: TFPColor);
    {** Extras RGBA channels }
    procedure ToRGB(out ARed,AGreen,ABlue,AAlpha: Byte); overload;
    {** Extras RGB channels }
    procedure ToRGB(out ARed,AGreen,ABlue: Byte); overload;
    {** Converts to TColor }
    function ToColor: TColor;
    {** Converts to a CSS string representation }
    function ToString: string;
    {** Compute grayscale equivalent }
    function ToGrayscale(AGammaCorrection: boolean = true): TBGRAPixel;
    {** Converts to TFPColor (16-bit per channel) }
    function ToFPColor: TFPColor;
    {** Check whether all channels are equal (this is generally not necessary to
        use this fonction unless the RGB value of a transparent color matter) }
    function EqualsExactly(constref AColor: TBGRAPixel): boolean;
    {** Assign a TBGRAPixel to a TColor }
    class Operator := (Source: TBGRAPixel): TColor;
    {** Assign a TColor to a TBGRAPixel }
    class Operator := (Source: TColor): TBGRAPixel;
    {** Intensity of the color (how much channels are saturated) }
    property Intensity: word read GetClassIntensity write SetClassIntensity;
    {** Lightness of the color (the resulting lightness of combined channels) }
    property Lightness: word read GetClassLightness write SetClassLightness;
  end;

  {** Checks if two pixels are equal. If they are both transparent,
      RGB values are ignored }
  operator = (const c1, c2: TBGRAPixel): boolean; inline;

const
  {$IFDEF BGRABITMAP_RGBAPIXEL}
  TBGRAPixel_RGBAOrder = True;
  TBGRAPixel_RedByteOffset = 0;
  TBGRAPixel_GreenByteOffset = 1;
  TBGRAPixel_BlueByteOffset = 2;
  {$ELSE}
  {** Order of the channels (when BGRA) }
  TBGRAPixel_RGBAOrder = False;
  {** Offset of the blue channel (when BGRA) }
  TBGRAPixel_BlueByteOffset = 0;
  {** Offset of the green channel (when BGRA) }
  TBGRAPixel_GreenByteOffset = 1;
  {** Offset of the blue channel (when BGRA) }
  TBGRAPixel_RedByteOffset = 2;
  {$ENDIF}
  {** Offset of the alpha channel }
  TBGRAPixel_AlphaByteOffset = 3;
  {$IFDEF ENDIAN_LITTLE}
  { Bit shift for the red channel }
  TBGRAPixel_RedShift = TBGRAPixel_RedByteOffset*8;
  { Bit shift for the green channel }
  TBGRAPixel_GreenShift = TBGRAPixel_GreenByteOffset*8;
  { Bit shift for the blue channel }
  TBGRAPixel_BlueShift = TBGRAPixel_BlueByteOffset*8;
  { Bit shift for the alpha channel }
  TBGRAPixel_AlphaShift = TBGRAPixel_AlphaByteOffset*8;
  {$ELSE}
  TBGRAPixel_RedShift = 24 - TBGRAPixel_RedByteOffset*8;
  TBGRAPixel_GreenShift = 24 - TBGRAPixel_GreenByteOffset*8;
  TBGRAPixel_BlueShift = 24 - TBGRAPixel_BlueByteOffset*8;
  TBGRAPixel_AlphaShift = 24 - TBGRAPixel_AlphaByteOffset*8;
  {$ENDIF}

  {* Creates a pixel with given RGBA values }
  function BGRA(red, green, blue, alpha: byte): TBGRAPixel; overload; inline;
  {* Creates a opaque pixel with given RGB values }
  function BGRA(red, green, blue: byte): TBGRAPixel; overload; inline;
  {* Returns the intensity of a pixel. The intensity is the
     maximum value reached by any component }
  function GetIntensity(c: TBGRAPixel): word; inline;
  {* Sets the intensity of a pixel }
  function SetIntensity(c: TBGRAPixel; intensity: word): TBGRAPixel;
  {* Returns the lightness of a pixel. The lightness is the
     perceived brightness, 0 being black and 65535 being white }
  function GetLightness(c: TBGRAPixel): word; overload;
  {* Sets the lightness of a pixel }
  function SetLightness(c: TBGRAPixel; lightness: word): TBGRAPixel; overload;
  {* Sets the lightness quickly, by fading towards black if _lightness_ is
      less than 32768, and fading towards white if _lightness_ is more
      than 32768 }
  function ApplyLightnessFast(color: TBGRAPixel; lightness: word): TBGRAPixel; inline;
  {* Sets the intensity quickly, by fading towards black if _lightness_ is
      less than 32768, and multiplying all components if _lightness_ is more
      than 32768. In case of saturation, it fades towards white }
  function ApplyIntensityFast(color: TBGRAPixel; lightness: LongWord): TBGRAPixel;
  {* Combines two lightnesses together. A value of 32768 is neutral. The
      Result may exceed 65535 }
  function CombineLightness(lightness1,lightness2: Int32or64): Int32or64;
  {* Converts a color into grayscale }
  function BGRAToGrayscale(c: TBGRAPixel): TBGRAPixel;
  {* Converts a color into grayscale assuming linear colorspace }
  function BGRAToGrayscaleLinear(c: TBGRAPixel): TBGRAPixel;
  {* Create a gray color with the given _lightness_ }
  function GrayscaleToBGRA(lightness: word): TBGRAPixel;
  {* Merge two colors without gamma correction }
  function MergeBGRA(c1, c2: TBGRAPixel): TBGRAPixel; overload;
  {* Merge two colors without gamma correction. _weight1_ and _weight2_
      indicates the weight of the color barycentre }
  function MergeBGRA(c1: TBGRAPixel; weight1: integer; c2: TBGRAPixel; weight2: integer): TBGRAPixel; overload;
  {* Merge two colors with gamma correction. _weight1_ and _weight2_
      indicates the weight of the color barycentre }
  function MergeBGRAWithGammaCorrection(c1: TBGRAPixel; weight1: byte; c2: TBGRAPixel; weight2: byte): TBGRAPixel;
  {* Converts a TColor value into an opaque pixel }
  function ColorToBGRA(color: TColor): TBGRAPixel; overload;
  {* Converts a TColor value into a pixel with given _opacity_ }
  function ColorToBGRA(color: TColor; opacity: byte): TBGRAPixel; overload;
  {* Converts a pixel into a TColor value, discarding the alpha value }
  function BGRAToColor(c: TBGRAPixel): TColor;
  {* Fonction to divide by 257 (not quite the same as **shr** 8)}
  function FastRoundDiv257(valueWord: LongWord): byte; inline;
  {* Converts a TFPColor value into a pixel. Note that even if
      TFPColor have 16-bit values, they are not considered as
      gamma expanded }
  function FPColorToBGRA(AValue: TFPColor): TBGRAPixel;
  {* Converts a pixel into a TFPColor }
  function BGRAToFPColor(AValue: TBGRAPixel): TFPColor; inline;
  {* Converts a 16-bit packed color into a TBGRAPixel }
  function Color16BitToBGRA(AColor: Word): TBGRAPixel;
  {* Converts a TBGRAPixel into a 16-bit packed color }
  function BGRAToColor16Bit(const AColor: TBGRAPixel): Word;
  {* Computes the difference (with gamma correction) between two pixels,
      taking into account all dimensions, including transparency. The
      Result ranges from 0 to 65535 }
  function BGRAWordDiff(c1, c2: TBGRAPixel): word;
  {* Computes the difference (with gamma correction) between two pixels,
      taking into account all dimensions, including transparency. The
      Result ranges from 0 to 255 }
  function BGRADiff(c1, c2: TBGRAPixel): byte;
  {* Fast computation of difference, without weighing the alpha channel and without gamma correction}
  function FastBGRALinearDiff(c1,c2: TBGRAPixel): byte;
  {* Fast computation of difference, without weighing the alpha channel }
  function FastBGRAExpandedDiff(c1,c2: TBGRAPixel): word;

  {* Get height [0..1] stored in a TBGRAPixel }
  function MapHeight(Color: TBGRAPixel): Single;

  {* Get TBGRAPixel to store height [0..1] }
  function MapHeightToBGRA(Height: Single; Alpha: Byte): TBGRAPixel;

type
  {* Array of TBGRAPixel to be used as a buffer }
  TBGRAPixelBuffer = packed array of TBGRAPixel;

  {** Allocate a buffer of TBGRAPixel }
  procedure AllocateBGRAPixelBuffer(var ABuffer: TBGRAPixelBuffer; ASize: integer);

type
  {** Array of pixels, not to be used as a buffer }
  ArrayOfTBGRAPixel = array of TBGRAPixel;
  {** Merge given colors without gamma correction }
  function MergeBGRA(const colors: array of TBGRAPixel): TBGRAPixel; overload;

type
  {* Possible modes when drawing a pixel over another one }
  TDrawMode = (
    {** The pixel is replaced }
    dmSet,
    {** The pixel is replaced if the pixel over has an alpha value of 255 }
    dmSetExceptTransparent,
    {** The pixel is blend over the other one according to alpha values,
        however no gamma correction is applied. In other words, the color
        space is assumed to be linear }
    dmLinearBlend,
    {** The pixel is blend over the other one according to alpha values,
        and a gamma correction is applied. In other word, the color
        space is assumed to be sRGB }
    dmDrawWithTransparency,
    {** Values of all channels are combined with Xor. This is useful to
        compute the binary difference, however it is not something that makes
        much sense to display on the screen }
    dmXor);

const
  {** An alias for the linear blend, because it is faster than blending
      with gamma correction }
  dmFastBlend = dmLinearBlend;

type
  {* Advanced blending modes. See [Paint.NET blend modes](http://www.brighthub.com/multimedia/photography/articles/18301.aspx)
    and [Formulas](http://www.pegtop.net/delphi/articles/blendmodes/). Blending layers has two steps. The first one is
    to apply the blend operations listed below, and the second is the actual merging of the colors }
  TBlendOperation = (
    {** Simple blend, except that it forces a linear merge so it is equivalent to _dmLinearBlend_ }
    boLinearBlend,
    {** Simple blend. It is equivalent to _dmLinearBlend_ or _dmDrawWithTransparency_ }
    boTransparent,
    {** Lighting blend modes (tends to increase the luminosity) }
    boLighten, boScreen, boAdditive, boLinearAdd, boColorDodge, boDivide, boNiceGlow, boSoftLight, boHardLight,
    {** Masking blend modes (tends to decrease the luminosity) }
    boGlow, boReflect, boOverlay, boDarkOverlay, boDarken, boMultiply, boColorBurn,
    {** Difference blend modes }
    boDifference, boLinearDifference, boExclusion, boLinearExclusion, boSubtract, boLinearSubtract, boSubtractInverse, boLinearSubtractInverse,
    {** Negation blend modes }
    boNegation, boLinearNegation,
    {** Xor blend mode. It is sightly different from _dmXor_ because the alpha value is used like in other blends modes }
    boXor,
    {** SVG implementation of soft light }
    boSvgSoftLight,
    {** Apply mask }
    boMask,
    {** Standard HSL colorspace }
    boLinearMultiplySaturation, boLinearHue, boLinearColor, boLinearLightness, boLinearSaturation,
    {** Corrected HSL colorspace (GSB) }
    boCorrectedHue, boCorrectedColor, boCorrectedLightness, boCorrectedSaturation
    );

const
  {** Alias to glow that express that this blend mode masks the part where the top layer is black }
  boGlowMask = boGlow;
  {** Alias because linear or non linear multiply modes are identical }
  boLinearMultiply = boMultiply;
  {** Alias to express that dark overlay is simply an overlay with gamma correction }
  boNonLinearOverlay = boDarkOverlay;

const
  {** String constants for blend modes }
  BlendOperationStr : array[TBlendOperation] of string
  = ('LinearBlend', 'Transparent',
     'Lighten', 'Screen', 'Additive', 'LinearAdd', 'ColorDodge', 'Divide', 'NiceGlow', 'SoftLight', 'HardLight',
     'Glow', 'Reflect', 'Overlay', 'DarkOverlay', 'Darken', 'Multiply', 'ColorBurn',
     'Difference', 'LinearDifference', 'Exclusion', 'LinearExclusion', 'Subtract', 'LinearSubtract', 'SubtractInverse', 'LinearSubtractInverse',
     'Negation', 'LinearNegation', 'Xor', 'SvgSoftLight', 'Mask', 'LinearMultiplySaturation',
     'LinearHue', 'LinearColor', 'LinearLightness', 'LinearSaturation',
     'CorrectedHue', 'CorrectedColor', 'CorrectedLightness', 'CorrectedSaturation');

  {** Returns the blend mode expressed by the string }
  function StrToBlendOperation(str: string): TBlendOperation;

type
  {* Specifies how a palette handles the alpha channel }
  TAlphaChannelPaletteOption = (
    {** The alpha channel is ignored. The alpha channel is considered to be stored elsewhere }
    acIgnore,
    {** One entry is allocated the fully transparent color }
    acTransparentEntry,
    {** The alpha channel is fully embedded in the palette so that a color is identified by its four RGBA channels }
    acFullChannelInPalette);

  {* Dithering algorithms that specifies how to handle colors that are not found in the palette }
  TDitheringAlgorithm = (
    {** The nearest color is to be used instead }
    daNearestNeighbor,
    {** The nearest color may be used however another color may be used to compensate for the error,
        following Floyd-Steinberg algorithm }
    daFloydSteinberg);

{$DEFINE INCLUDE_INTERFACE}
{$i basiccolorspace.inc}

{$DEFINE INCLUDE_INTERFACE}
{$i extendedcolorspace.inc}

{$ENDIF}

{$IFDEF INCLUDE_IMPLEMENTATION}
{$UNDEF INCLUDE_IMPLEMENTATION}

{$DEFINE INCLUDE_IMPLEMENTATION}
{$i basiccolorspace.inc}

{$DEFINE INCLUDE_IMPLEMENTATION}
{$i extendedcolorspace.inc}

function StrToBlendOperation(str: string): TBlendOperation;
var op: TBlendOperation;
begin
  Result := boTransparent;
  str := LowerCase(str);
  for op := low(TBlendOperation) to high(TBlendOperation) do
    if str = LowerCase(BlendOperationStr[op]) then
    begin
      Result := op;
      exit;
    end;
end;

{************************** Color functions **************************}

procedure AllocateBGRAPixelBuffer(var ABuffer: TBGRAPixelBuffer; ASize: integer);
begin
  if ASize > length(ABuffer) then
    setlength(ABuffer, max(Length(ABuffer) * 2, ASize));
end;

function BGRA(red, green, blue, alpha: byte): TBGRAPixel;
begin
  LongWord(Result) := (red shl TBGRAPixel_RedShift) or
                      (green shl TBGRAPixel_GreenShift) or
                      (blue shl TBGRAPixel_BlueShift) or
                      (alpha shl TBGRAPixel_AlphaShift);
end;

function BGRA(red, green, blue: byte): TBGRAPixel; overload;
begin
  LongWord(Result) := (red shl TBGRAPixel_RedShift) or
                      (green shl TBGRAPixel_GreenShift) or
                      (blue shl TBGRAPixel_BlueShift) or
                      (255 shl TBGRAPixel_AlphaShift);
end;

operator = (const c1, c2: TBGRAPixel): boolean;
begin
  if (c1.alpha = 0) and (c2.alpha = 0) then
    Result := True
  else
    Result := (c1.alpha = c2.alpha) and (c1.red = c2.red) and
      (c1.green = c2.green) and (c1.blue = c2.blue);
end;

function GetIntensity(c: TBGRAPixel): word;
begin
  Result := c.red;
  if c.green > Result then
    Result := c.green;
  if c.blue > Result then
    Result := c.blue;
  Result := GammaExpansionTab[Result];
end;

function SetIntensity(c: TBGRAPixel; intensity: word): TBGRAPixel;
begin
  Result := GammaCompression(SetIntensity(GammaExpansion(c),intensity));
end;

function GetLightness(c: TBGRAPixel): word;
begin
  Result := GetLightness(GammaExpansion(c));
end;

function SetLightness(c: TBGRAPixel; lightness: word): TBGRAPixel;
begin
  Result := GammaCompression(SetLightness(GammaExpansion(c),lightness));
end;

function ApplyLightnessFast(color: TBGRAPixel; lightness: word): TBGRAPixel;
var
  r,g,b: word;
  lightness256: byte;
begin
  if lightness <= 32768 then
    begin
      if lightness = 32768 then
        Result := color else
      begin
        lightness256 := GammaCompressionTab[lightness shl 1];
        Result := BGRA(color.red   * lightness256 shr 8,
                       color.green * lightness256 shr 8,
                       color.blue  * lightness256 shr 8,
                       color.alpha);
      end;
    end
  else if lightness = 65535 then
    Result := BGRA(255,255,255,color.alpha)
  else
    begin
      dec(lightness, 32767);
      r := GammaExpansionTab[color.red];
      g := GammaExpansionTab[color.green];
      b := GammaExpansionTab[color.blue];
      Result := BGRA(GammaCompressionTab[ r + (not r)*lightness shr 15 ],
                     GammaCompressionTab[ g + (not g)*lightness shr 15 ],
                     GammaCompressionTab[ b + (not b)*lightness shr 15 ],
                     color.alpha);
    end;
end;

function ApplyIntensityFast(color: TBGRAPixel; lightness: LongWord): TBGRAPixel;
var
    maxValue,invMaxValue,r,g,b: LongWord;
    lightness256: byte;
begin
  if lightness <= 32768 then
    begin
      if lightness = 32768 then
        Result := color else
      begin
        lightness256 := GammaCompressionTab[lightness shl 1];
        Result := BGRA(color.red * lightness256 shr 8, color.green*lightness256 shr 8,
                       color.blue * lightness256 shr 8, color.alpha);
      end;
    end
  else
    begin
      r := CombineLightness(GammaExpansionTab[color.red], lightness);
      g := CombineLightness(GammaExpansionTab[color.green], lightness);
      b := CombineLightness(GammaExpansionTab[color.blue], lightness);
      maxValue := r;
      if g > maxValue then maxValue := g;
      if b > maxValue then maxValue := b;
      if maxValue <= 65535 then
        Result := BGRA(GammaCompressionTab[r],
                       GammaCompressionTab[g],
                       GammaCompressionTab[b],
                       color.alpha)
      else
      begin
        invMaxValue := (LongWord(2147483647)+LongWord(maxValue-1)) div maxValue;
        maxValue := (maxValue-65535) shr 1;
        r := r*invMaxValue shr 15 + maxValue;
        g := g*invMaxValue shr 15 + maxValue;
        b := b*invMaxValue shr 15 + maxValue;
        if r >= 65535 then
          Result.red := 255
        else
          Result.red := GammaCompressionTab[r];
        if g >= 65535 then
          Result.green := 255
        else
          Result.green := GammaCompressionTab[g];
        if b >= 65535 then
          Result.blue := 255
        else
          Result.blue := GammaCompressionTab[b];
        Result.alpha := color.alpha;
      end;
    end;
end;

function CombineLightness(lightness1,lightness2: Int32or64): Int32or64;
{$ifdef CPUI386} {$asmmode intel} assembler;
  asm
    imul edx
    shl edx, 17
    shr eax, 15
    or edx, eax
    mov Result, edx
  end;
{$ELSE}
begin
  if (lightness1 < 0) xor (lightness2 < 0) then
    Result := -(int64(-lightness1)*lightness2 shr 15)
  else
    Result := int64(lightness1)*lightness2 shr 15;
end;
{$ENDIF}

// Conversion to grayscale by taking into account
// different color weights
function BGRAToGrayscale(c: TBGRAPixel): TBGRAPixel;
var
  ec:    TExpandedPixel;
  gray:  word;
  cgray: byte;
begin
  if c.alpha = 0 then
  begin
    Result := BGRAPixelTransparent;
    exit;
  end;
  //gamma expansion
  ec    := GammaExpansion(c);
  //gray composition
  gray  := (ec.red * redWeightShl10 + ec.green * greenWeightShl10 +
    ec.blue * blueWeightShl10 + 512) shr 10;
  //gamma compression
  cgray := GammaCompressionTab[gray];
  Result.red := cgray;
  Result.green := cgray;
  Result.blue := cgray;
  Result.alpha := c.alpha;
end;

function BGRAToGrayscaleLinear(c: TBGRAPixel): TBGRAPixel;
var
  gray:  byte;
begin
  if c.alpha = 0 then
  begin
    Result := BGRAPixelTransparent;
    exit;
  end;
  //gray composition
  gray  := (c.red * redWeightShl10 + c.green * greenWeightShl10 +
    c.blue * blueWeightShl10 + 512) shr 10;
  //gamma compression
  Result.red := gray;
  Result.green := gray;
  Result.blue := gray;
  Result.alpha := c.alpha;
end;

function GrayscaleToBGRA(lightness: word): TBGRAPixel;
begin
  Result.red := GammaCompressionTab[lightness];
  Result.green := Result.red;
  Result.blue := Result.red;
  Result.alpha := $ff;
end;

{ Merge linearly two colors of same importance }
function MergeBGRA(c1, c2: TBGRAPixel): TBGRAPixel;
var c12: LongWord;
begin
  if (c1.alpha = 0) then
    Result := c2
  else if (c2.alpha = 0) then
    Result := c1
  else
    begin
      c12 := c1.alpha + c2.alpha;
      Result.red   := (c1.red * c1.alpha + c2.red * c2.alpha + c12 shr 1) div c12;
      Result.green := (c1.green * c1.alpha + c2.green * c2.alpha + c12 shr 1) div c12;
      Result.blue  := (c1.blue * c1.alpha + c2.blue * c2.alpha + c12 shr 1) div c12;
      Result.alpha := (c12 + 1) shr 1;
    end;
end;

function MergeBGRA(c1: TBGRAPixel; weight1: integer; c2: TBGRAPixel;
  weight2: integer): TBGRAPixel;
var
    f1,f2,f12: int64;
begin
  if (weight1 = 0) or (c1.alpha = 0) then
    begin
      if (weight2 = 0) or (c2.alpha = 0) then
        Result := BGRAPixelTransparent
      else
        Result := c2
    end
  else if (weight2 = 0) or (c2.alpha = 0) then
    Result := c1
  else if (weight1+weight2 = 0) then
    Result := BGRAPixelTransparent
  else
    begin
      f1 := int64(c1.alpha)*weight1;
      f2 := int64(c2.alpha)*weight2;
      f12 := f1+f2;
      if f12 = 0 then
        Result := BGRAPixelTransparent
      else
        begin
          Result.red   := (c1.red * f1 + c2.red * f2 + f12 shr 1) div f12;
          Result.green := (c1.green * f1 + c2.green * f2 + f12 shr 1) div f12;
          Result.blue  := (c1.blue * f1 + c2.blue * f2 + f12 shr 1) div f12;
          {$hints off}
          Result.alpha := (f12 + ((weight1+weight2) shr 1)) div (weight1+weight2);
          {$hints on}
        end;
    end;
end;

function MergeBGRAWithGammaCorrection(c1: TBGRAPixel; weight1: byte; c2: TBGRAPixel;
  weight2: byte): TBGRAPixel;
var
    w1,w2,f1,f2,f12,a: UInt32or64;
begin
  w1 := weight1;
  w2 := weight2;
  if (w1 = 0) or (c1.alpha = 0) then
    begin
      if (w2 = 0) or (c2.alpha = 0) then
        Result := BGRAPixelTransparent
      else
        Result := c2
    end
  else if (w2 = 0) or (c2.alpha = 0) then
    Result := c1
  else
    begin
      f1 := c1.alpha*w1;
      f2 := c2.alpha*w2;
      a := (f1+f2 + ((w1+w2) shr 1)) div (w1+w2);
      if a = 0 then
        begin
          Result := BGRAPixelTransparent;
          exit;
        end
      else
        Result.alpha := a;
      {$IFNDEF CPU64}
      if (f1 >= 32768) or (f2 >= 32768) then
      begin
        f1 := f1 shr 1;
        f2 := f2 shr 1;
      end;
      {$ENDIF}
      f12 := f1+f2;
      Result.red   := GammaCompressionTab[(GammaExpansionTab[c1.red] * f1 + GammaExpansionTab[c2.red] * f2) div f12];
      Result.green := GammaCompressionTab[(GammaExpansionTab[c1.green] * f1 + GammaExpansionTab[c2.green] * f2) div f12];
      Result.blue  := GammaCompressionTab[(GammaExpansionTab[c1.blue] * f1 + GammaExpansionTab[c2.blue] * f2) div f12];
  end;
end;

{ Convert a TColor value to a TBGRAPixel value }
{$PUSH}{$R-}
function ColorToBGRA(color: TColor): TBGRAPixel; overload;
begin
  if (color < 0) or (color > $ffffff) then color := ColorToRGB(color);
  RedGreenBlue(color, Result.red,Result.green,Result.blue);
  Result.alpha := 255;
end;

function ColorToBGRA(color: TColor; opacity: byte): TBGRAPixel; overload;
begin
  if (color < 0) or (color > $ffffff) then color := ColorToRGB(color);
  RedGreenBlue(color, Result.red,Result.green,Result.blue);
  Result.alpha := opacity;
end;
{$POP}

function BGRAToColor(c: TBGRAPixel): TColor;
begin
  Result := RGBToColor(c.red, c.green, c.blue);
end;

function FastRoundDiv257(valueWord: LongWord): byte; inline;
begin
  Result := LongWord(valueWord + 127 - (valueWord shr 8)) shr 8;
end;

{ Conversion from TFPColor to TBGRAPixel assuming TFPColor
  is already gamma compressed }
function FPColorToBGRA(AValue: TFPColor): TBGRAPixel;
begin
  with AValue do
    Result := BGRA(FastRoundDiv257(red), FastRoundDiv257(green), FastRoundDiv257(blue), alpha shr 8);
end;

function BGRAToFPColor(AValue: TBGRAPixel): TFPColor; inline;
begin
  Result.red := AValue.red shl 8 + AValue.red;
  Result.green := AValue.green shl 8 + AValue.green;
  Result.blue := AValue.blue shl 8 + AValue.blue;
  Result.alpha := AValue.alpha shl 8 + AValue.alpha;
end;

function Color16BitToBGRA(AColor: Word): TBGRAPixel;
begin
  Result := BGRA( ((AColor and $F800) shr 11)*255 div 31,
                  ((AColor and $07e0) shr 5)*255 div 63,
                   (AColor and $001f)*255 div 31 );
end;

function BGRAToColor16Bit(const AColor: TBGRAPixel): Word;
begin
  Result := (((AColor.Red * 31 + 64) div 255) shl 11) +
            (((AColor.green * 63 + 64) div 255) shl 5) +
             ((AColor.blue * 31 + 64) div 255);
end;

function BGRAWordDiff(c1, c2: TBGRAPixel): word;
begin
  if c1 = c2 then
    Result := 0
  else
    Result := ExpandedDiff(GammaExpansion(c1),GammaExpansion(c2));
end;

function BGRADiff(c1,c2: TBGRAPixel): byte;
begin
  if c1 = c2 then
    Result := 0
  else
    Result := ExpandedDiff(GammaExpansion(c1),GammaExpansion(c2)) shr 8;
end;

function FastBGRALinearDiff(c1, c2: TBGRAPixel): byte;
begin
  Result := max(min((abs(c1.red-c2.red)+(abs(c1.green-c2.green) shl 1)+abs(c1.blue-c2.blue)) shr 2,
             min(c1.alpha,c2.alpha)), abs(c1.alpha-c2.alpha));
end;

function FastBGRAExpandedDiff(c1, c2: TBGRAPixel): word;
var wa1,wa2: word;
begin
  wa1 := c1.alpha shl 8 + c1.alpha;
  wa2 := (c2.alpha shl 8) + c2.alpha;
  Result := max(min((abs(GammaExpansionTab[c1.red]-GammaExpansionTab[c2.red])+
               (abs(GammaExpansionTab[c1.green]-GammaExpansionTab[c2.green]) shl 1)+
               abs(GammaExpansionTab[c1.blue]-GammaExpansionTab[c2.blue])) shr 2,
             min(wa1,wa2)),
             abs(wa1-wa2));
end;

function MergeBGRA(const colors: array of TBGRAPixel): TBGRAPixel;
var
  sumR,sumG,sumB,sumA: UInt32or64;
  i: integer;
begin
  if length(colors)<=0 then
  begin
    Result := BGRAPixelTransparent;
    exit;
  end;
  sumR := 0;
  sumG := 0;
  sumB := 0;
  sumA := 0;
  for i := 0 to high(colors) do
  with colors[i] do
  begin
    inc(sumR, red*alpha);
    inc(sumG, green*alpha);
    inc(sumB, blue*alpha);
    inc(sumA, alpha);
  end;
  if sumA > 0 then
  begin
    Result.red := (sumR + sumA shr 1) div sumA;
    Result.green := (sumG + sumA shr 1) div sumA;
    Result.blue := (sumB + sumA shr 1) div sumA;
    Result.alpha := sumA div LongWord(length(colors));
  end
  else
    Result := BGRAPixelTransparent;
end;

function MapHeight(Color: TBGRAPixel): Single;
var intval: integer;
begin
  intval := color.Green shl 16 + color.red shl 8 + color.blue;
  Result := intval * 5.960464832810452e-8;
end;

function MapHeightToBGRA(Height: Single; Alpha: Byte): TBGRAPixel;
var intval: integer;
begin
  if Height >= 1 then
    Result := BGRA(255, 255, 255, alpha)
  else if Height <= 0 then
    Result := BGRA(0, 0, 0, alpha)
  else
    begin
      intval := round(Height*16777215);
      {$PUSH}{$R-}
      Result := BGRA(intval shr 8,intval shr 16,intval,alpha);
      {$POP}
    end;
end;
{$ENDIF}

{$IFDEF INCLUDE_INIT}
{$UNDEF INCLUDE_INIT}
  BGRASetGamma();

  {$DEFINE INCLUDE_INITIALIZATION}
  {$i extendedcolorspace.inc}
{$ENDIF}
