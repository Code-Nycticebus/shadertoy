# Shadertoy

This is just an example project using [claymore](https://github.com/Code-Nycticebus/claymore).

## Setup

```terminal
git clone https://github.com/Code-Nycticebus/shadertoy --recurse-submodules 
```

Then you can run it with [pybuildc](https://github.com/Code-Nycticebus/pybuildc):

```terminal
pybuildc run
```

## Uniforms

These are the provided uniforms:
```glsl
uniform vec3  iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform float iFrame;
uniform vec4  iMouse;
```

