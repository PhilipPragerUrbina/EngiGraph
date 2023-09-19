//
// Created by Philip on 9/18/2023.
//

#include "ObjLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyOBJLoader/tiny_obj_loader.h"
#include "FileUtils.h"
#include "../Exceptions/RuntimeException.h"
namespace EngiGraph {

    std::vector<VisualMesh> loadOBJ(const std::string& filename){

        validateFileExistence(filename);
        validateFileExtensions(filename,{".obj"});

        tinyobj::ObjReaderConfig reader_config;
        reader_config.triangulate = true;
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filename, reader_config)) {
            throw RuntimeException("Problem reading OBJ file : " +  reader.Error());
        }

        if (!reader.Warning().empty()) {
            //todo warning system
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();

        std::vector<VisualMesh> meshes;

        for (const auto& shape : shapes) {

            VisualMesh mesh;
            mesh.name = shape.name;

            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

                bool face_generate_normals = false;
                bool face_generate_texture_coords = false;

                for (size_t v = 0; v < 3; v++) {

                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                    tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
                    mesh.positions.emplace_back(vx,vy,vz);

                    mesh.indices.push_back(index_offset + v); //This loader has per attribute indices, so there is no proper indexing scheme.

                    if (idx.normal_index >= 0 && !face_generate_normals) {
                        tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                        tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                        tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                        mesh.normals.emplace_back(nx,ny,nz);
                        mesh.normals[mesh.normals.size()-1].normalize();
                    }else {
                        face_generate_normals = true;
                    }

                    if (idx.texcoord_index >= 0 && !face_generate_texture_coords) {
                        tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                        tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                        mesh.uv_coordinates.emplace_back(tx,ty);
                    }else{
                        face_generate_texture_coords = true;
                    }

                }

                if(face_generate_normals){
                    //auto generates flat normals
                    Eigen::Vector3f flat_normal = (mesh.positions[mesh.positions.size()-2] - mesh.positions[mesh.positions.size()-3]).cross(mesh.positions[mesh.positions.size()-3] - mesh.positions[mesh.positions.size()-1]);
                    flat_normal.normalize();
                    mesh.normals.push_back(flat_normal);
                    mesh.normals.push_back(flat_normal);
                    mesh.normals.push_back(flat_normal);
                }
                if(face_generate_texture_coords){
                    mesh.uv_coordinates.emplace_back(0,1);
                    mesh.uv_coordinates.emplace_back(1,0);
                    mesh.uv_coordinates.emplace_back(1,1);
                }
                index_offset += 3;
            }
            meshes.push_back(mesh);
            if(!mesh.validate()){
                throw RuntimeException("OBJ mesh data not valid : " + filename);
            }
        }
        return meshes;
    }

} // EngiGraph