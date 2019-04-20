#Obj to C header converter

Converts the obj 3D model into a C header file including the model vertex data in the following format (not present attributes are omited):

```
struct ModelVertex {
float position[3];
float normal[3];
float uv[2];
};

struct ModelVertex const modelVertices[N] = {...
const unsigned int modelIndices[M] = {...
```

Vertex deduplication is performed so M >= N

##Build

```
wget https://github.com/syoyo/tinyobjloader/raw/master/tiny_obj_loader.h
make
```

##Usage

```
./obj2header input.obj output.h
```
