
target_sources(SkapeEngine
  PRIVATE
    Transform.cpp
    Types.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Math.h
      Matrix.h
      Matrix3x3.h
      Matrix4x4.h
      Matrix_helper.h
      Transform.h
      Types.h
      Vector.h
      Vector2.h
      Vector3.h
      Vector4.h
      VectorSwizzle.h
)