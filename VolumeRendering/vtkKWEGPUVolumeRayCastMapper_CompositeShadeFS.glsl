//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2008 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the GNU General Public License 
//   version 3 as published by the Free Software Foundation and appearing in 
//   the file LICENSE.txt included in the top level directory of this source
//   code distribution. Alternatively you may (at your option) use any later 
//   version of the GNU General Public License if such license has been 
//   publicly approved by Kitware, Inc. (or its successors, if any).
//
//   VTKEdge is distributed "AS IS" with NO WARRANTY OF ANY KIND, INCLUDING
//   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
//   PURPOSE. See LICENSE.txt for additional details.
//
//   VTKEdge is available under alternative license terms. Please visit
//   vtkedge.org or contact us at kitware@kitware.com for further information.
//
//=============================================================================

// Fragment program part with ray cast and composite+shade method.

uniform sampler3D dataSetTexture;
uniform sampler1D colorTexture;
uniform sampler1D opacityTexture;

// Change-of-coordinate matrix from eye space to texture space
uniform mat3 eyeToTexture3;
uniform mat4 eyeToTexture4;

// Tranpose of Change-of-coordinate matrix from texture space to eye space
uniform mat3 transposeTextureToEye;

uniform vec3 lowBounds;
uniform vec3 highBounds;

// Used to compute the gradient.
uniform vec3 cellStep;
uniform vec3 cellScale;

// Entry position (global scope)
vec3 pos;
// Incremental vector in texture space (global scope)
vec3 rayDir;

float tMax;

vec4 initialColor();

void trace(void)
{
  vec3 xvec=vec3(cellStep.x,0.0,0.0); // 0.01
  vec3 yvec=vec3(0.0,cellStep.y,0.0);
  vec3 zvec=vec3(0.0,0.0,cellStep.z);
  const vec3 minusOne=vec3(-1.0,-1.0,-1.0);
  const vec4 clampMin=vec4(0.0,0.0,0.0,0.0);
  const vec4 clampMax=vec4(1.0,1.0,1.0,1.0);
  
  // Reverse ray direction in eye space
  vec3 wReverseRayDir=eyeToTexture3*rayDir;
  wReverseRayDir=wReverseRayDir*minusOne;
  wReverseRayDir=normalize(wReverseRayDir);
  
  vec3 lightPos;
  vec3 ldir;
  vec3 h;
  vec4 hPos; // homogeneous position
  // Directonal light: w==0
  if(gl_LightSource[0].position.w==0.0)
    {
    ldir=gl_LightSource[0].position.xyz;
    ldir=normalize(ldir);
    h=normalize(ldir+wReverseRayDir);
    }
  else
    {
    lightPos=gl_LightSource[0].position.xyz/gl_LightSource[0].position.w;
    hPos.w=1.0; // used later
    }
  
  // Max intensity is the lowest value.
  vec4 destColor=initialColor();
  float remainOpacity=1.0-destColor.a;
  
  bool inside=true;
  
  vec4 scalarValue;
  vec4 color;
  vec4 opacity;
  vec4 sample;
  vec3 g1;
  vec3 g2;
  vec4 tmp;
  float shininessFactor;
  float sqrDistance;

  float att;
  vec4 finalColor;
  float spot;
  // We NEED two nested while loops. It is trick to work around hardware
  // limitation about the maximum number of loops.
  
  float t=0.0;
  while(inside)
    {
    while(inside)
      {
      // scalarValue is the sampled texture value in the volume at pos
      scalarValue=texture3D(dataSetTexture,pos);
      // opacity is the sampled texture value in the 1D opacity texture at
      // scalarValue
      opacity=texture1D(opacityTexture,scalarValue.x);
      if(opacity.a>0.0)
        {
        g1.x=texture3D(dataSetTexture,pos+xvec).x;
        g1.y=texture3D(dataSetTexture,pos+yvec).x;
        g1.z=texture3D(dataSetTexture,pos+zvec).x;
        g2.x=texture3D(dataSetTexture,pos-xvec).x;
        g2.y=texture3D(dataSetTexture,pos-yvec).x;
        g2.z=texture3D(dataSetTexture,pos-zvec).x;
        // g1-g2 is  the gradient in texture coordinates
        // the result is the normalized gradient in eye coordinates.
        
        g2=g1-g2;
        g2=g2*cellScale;
        
        float normalLength=length(g2);
        if(normalLength>0.0)
          {
          g2=normalize(transposeTextureToEye*g2);
          }
        else
          {
          g2=vec3(0.0,0.0,0.0);
          }
        
        // color is the sampled texture value in the 1D color texture at
        // scalarValue
        color=texture1D(colorTexture,scalarValue.x);

        // initialize color to 0.0
        finalColor = 0.0;        

        if(gl_LightSource[0].position.w!=0.0)
          {
          // We need to know the eye position only if light is positional
          // ldir= vertex position in eye coordinates
          hPos.xyz=pos;
          tmp=eyeToTexture4*hPos;
          ldir=tmp.xyz/tmp.w;
          // ldir=light direction
          ldir=lightPos-ldir;
          sqrDistance=dot(ldir,ldir);
          ldir=normalize(ldir);
          h=normalize(ldir+wReverseRayDir);
          att=1.0/(gl_LightSource[0].constantAttenuation+gl_LightSource[0].linearAttenuation*sqrt(sqrDistance)+gl_LightSource[0].quadraticAttenuation*sqrDistance);
          }
        else
          {
          att=1.0;
          }
        
        if(att>0.0)
          {
          if(gl_LightSource[0].spotCutoff==180.0)
            {
            spot=1.0;
            }
          else
            {
            float coef=-dot(ldir,gl_LightSource[0].spotDirection);
            if(coef>=gl_LightSource[0].spotCosCutoff)
              {
              spot=pow(coef,gl_LightSource[0].spotExponent);
              }
            else
              {
              spot=0.0;
              }
            }
          if(spot>0.0)
            {
            // LIT operation...
            float nDotL=dot(g2,ldir);
            float nDotH=dot(g2,h);
            
            // separate nDotL and nDotH for two-sided shading, otherwise we
            // get black spots.
            
            if(nDotL<0.0) // two-sided shading
              {
              nDotL=-nDotL;
              }
            
            if(nDotH<0.0) // two-sided shading
              {
              nDotH=-nDotH;
              }
            // ambient term for this light
            finalColor+=gl_FrontLightProduct[0].ambient;
            
            // diffuse term for this light
            if(nDotL>0.0)
              {
              finalColor+=(gl_FrontLightProduct[0].diffuse*nDotL)*color;
              }
            
            // specular term for this light
            shininessFactor=pow(nDotH,gl_FrontMaterial.shininess);
            finalColor+=gl_FrontLightProduct[0].specular*shininessFactor;
            finalColor*=att*spot;
            }
          }
        
        // scene ambient term
        finalColor+=gl_FrontLightModelProduct.sceneColor*color;

        // clamp. otherwise we get black spots
        finalColor=clamp(finalColor,clampMin,clampMax);
        
        finalColor=finalColor*opacity.a;
        destColor+=finalColor*remainOpacity;
        remainOpacity=remainOpacity*(1.0-opacity.w);
        } // opacity
      pos=pos+rayDir;
      t+=1.0;
      inside=t<tMax && all(greaterThanEqual(pos,lowBounds))
        && all(lessThanEqual(pos,highBounds))
        && (remainOpacity>=0.0039); // 1/255=0.0039
      }
    }
  // clamp. otherwise we get black spots
  destColor=clamp(destColor,clampMin,clampMax);
  gl_FragColor=destColor;
  gl_FragColor.w=1.0-remainOpacity;
}
