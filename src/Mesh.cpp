#include "Mesh.h"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "stb_image.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures) {
  vertices_ = vertices;
  indices_ = indices;
  textures_ = textures;

  setupMesh();
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
               &indices_[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, tex_coords));

  glBindVertexArray(0);
}

void Mesh::Draw(GLuint& program) {
  // bind appropriate textures
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  unsigned int normalNr = 1;
  unsigned int heightNr = 1;
  for (unsigned int i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 +
                    i);  // active proper texture unit before binding
    // retrieve texture number (the N in diffuse_textureN)
    std::string number;
    std::string name = textures_[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuseNr++);
    else if (name == "texture_specular")
      number = std::to_string(specularNr++);  // transfer unsigned int to string
    else if (name == "texture_normal")
      number = std::to_string(normalNr++);  // transfer unsigned int to string
    else if (name == "texture_height")
      number = std::to_string(heightNr++);  // transfer unsigned int to string

    // now set the sampler to the correct texture unit
    glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i);
    // and finally bind the texture
    glBindTexture(GL_TEXTURE_2D, textures_[i].id);
  }

  // draw mesh
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  glActiveTexture(GL_TEXTURE0);
}

void Texture::TextureFromFile(std::string_view file_path) {
  stbi_set_flip_vertically_on_load(is_uv_inverted);

  int width, height, nbrChannels;
  unsigned char* data =
      stbi_load(file_path.data(), &width, &height, &nbrChannels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image\n" << file_path.data() << "\n";
    // return texture;
  }

  std::cout << "width : " << width << " height : " << height
            << " Channels : " << nbrChannels << '\n';

  // Load Texture
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLint internalFormat = 0;
  if (nbrChannels == 1) {
    internalFormat = GL_RED;
  } else if (nbrChannels == 2) {
    internalFormat = GL_RG;
  } else if (nbrChannels == 3) {
    internalFormat = GL_RGB;
  } else if (nbrChannels == 4) {
    internalFormat = GL_RGBA;
  } else {
    std::cerr << "Texture channel nbr are not suitable\n";
  }

  //  GLint internalFormat = nbrChannels == 3 ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
               internalFormat, GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Texture::TextureFromFile(std::string_view file_path, bool is_inverted) {
  is_uv_inverted = is_inverted;
  TextureFromFile(file_path);
}

void Texture::TextureFromFileRepeat(std::string_view file_path) {
  stbi_set_flip_vertically_on_load(is_uv_inverted);

  int width, height, nbrChannels;
  unsigned char* data =
      stbi_load(file_path.data(), &width, &height, &nbrChannels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image\n" << file_path.data() << "\n";
    // return texture;
  }

  std::cout << "width : " << width << " height : " << height
            << " Channels : " << nbrChannels << '\n';

  // Load Texture
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLint internalFormat = 0;
  if (nbrChannels == 1) {
    internalFormat = GL_RED;
  } else if (nbrChannels == 2) {
    internalFormat = GL_RG;
  } else if (nbrChannels == 3) {
    internalFormat = GL_RGB;
  } else if (nbrChannels == 4) {
    internalFormat = GL_RGBA;
  } else {
    std::cerr << "Texture channel nbr are not suitable\n";
  }

  //  GLint internalFormat = nbrChannels == 3 ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
               internalFormat, GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Texture::HDRTextureFromFile(std::string_view file_path) {
  stbi_set_flip_vertically_on_load(is_uv_inverted);

  int width, height, nbrChannels;
  float* data = stbi_loadf(file_path.data(), &width, &height, &nbrChannels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image\n" << file_path.data() << "\n";
    // return texture;
  }
  std::cout << "width : " << width << " height : " << height
            << " Channels : " << nbrChannels << '\n';
  // Load Texture
  GLint internalFormat = nbrChannels == 3 ? GL_RGB16F : GL_RGBA16F;
  GLint format = nbrChannels == 3 ? GL_RGB : GL_RGBA;
  glGenTextures(1, &hdrTexture);
  glBindTexture(GL_TEXTURE_2D, hdrTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
               GL_FLOAT, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
}

void Texture::HDRTextureFromFile(std::string_view file_path, bool is_inverted) {
  is_uv_inverted = is_inverted;
  HDRTextureFromFile(file_path);
}

void Texture::HDRTextureFromFileRepeat(std::string_view file_path) {
  stbi_set_flip_vertically_on_load(is_uv_inverted);

  int width, height, nbrChannels;
  float* data = stbi_loadf(file_path.data(), &width, &height, &nbrChannels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image\n" << file_path.data() << "\n";
    // return texture;
  }
  std::cout << "width : " << width << " height : " << height
            << " Channels : " << nbrChannels << '\n';
  // Load Texture
  GLint internalFormat = nbrChannels == 3 ? GL_RGB16F : GL_RGBA16F;
  GLint format = nbrChannels == 3 ? GL_RGB : GL_RGBA;
  glGenTextures(1, &hdrTexture);
  glBindTexture(GL_TEXTURE_2D, hdrTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
               GL_FLOAT, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_image_free(data);
}

void Texture::BindTexture(GLenum textureUnit) {
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::BindTextureHDR(GLenum textureUnit) {
  glActiveTexture(textureUnit);

  glBindTexture(GL_TEXTURE_2D, hdrTexture);
}

void Texture::BindTextureInt(int textureUnit) {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, id);
}