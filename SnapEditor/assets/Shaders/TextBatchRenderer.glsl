#type vertex

#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int aEntityID;

uniform mat4 u_ProjectionView;

out vec2 v_TexCoords;
out vec4 v_Color;
out flat int v_EntityID;

void main()
{
   v_TexCoords = aTexCoords;
   v_Color = aColor;
   v_EntityID = aEntityID;
   gl_Position = u_ProjectionView * vec4(aPosition, 1.0);
}

#type fragment

#version 460 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int MousePickingID;
        
in vec2 v_TexCoords;
in vec4 v_Color;
in flat int v_EntityID;

uniform sampler2D u_TextureAtlas;

float screenPxRange()
{
	const float pxRange = 2.0; // set to distance field's pixel range
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_TextureAtlas, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(v_TexCoords);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
   vec4 texColor = v_Color * texture(u_TextureAtlas, v_TexCoords);
   vec3 msd = texture(u_TextureAtlas, v_TexCoords).rgb;

   float sd = median(msd.r, msd.g, msd.b);
   float screenPxDistance = screenPxRange()*(sd - 0.5);
   float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
   if (opacity == 0.0)
       discard;
   
   vec4 bgColor = vec4(0.0);
   FragColor = mix(bgColor, v_Color, opacity);
   if (FragColor.a == 0.0)
   		discard;

   MousePickingID = v_EntityID;
}