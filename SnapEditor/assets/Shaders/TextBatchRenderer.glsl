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

void main()
{
   vec4 SampleColor = vec4(1.0, 1.0, 1.0, texture(u_TextureAtlas, v_TexCoords).r);
   if(SampleColor.a < 0.1)
      discard;
   FragColor = v_Color * SampleColor;
   MousePickingID = v_EntityID;
}