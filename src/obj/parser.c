#include "obj/parser.h"
#include "base/io.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 65536

static inline bool is_space(const char c) {
  return (c == ' ') || (c == '\t') || (c == '\r');
}

static inline bool is_digit(const char c) { return (c >= '0') && (c <= '9'); }

static inline bool is_end_of_name(const char c) {
  return (c == '\t') || (c == '\r') || (c == '\n');
}

static const char *skip_space(const char *ptr) {
  for (; is_space(*ptr); ++ptr)
    ;
  return ptr;
}

static const char *skip_line(const char *ptr) {
  for (; *ptr != '\n'; ++ptr)
    ;
  return ++ptr;
}

static const char *end_of_name(const char *ptr) {
  for (; !is_end_of_name(*ptr); ++ptr)
    ;
  return ptr;
}

static error parse_position(const char** ptr_ptr, vector* verts, const int vert_count) {
  const char *ptr = ++*ptr_ptr;
  char *end = NULL;
  for (int i = 0; i < vert_count; ++i) {
    const ObjCoord vert = strtof(ptr, &end);
    if (end == ptr) {
      return kErrorReadingObjFile;
    }
    ObjCoord* vert_ptr = vector_push(verts, 1);
    if (vert_ptr == NULL) {
      return kErrorAllocationFailed;
    }
    *vert_ptr = vert;

    ptr = skip_space(end);
  }
  *ptr_ptr = ptr;
  return kErrorNil;
}

static error parse_facet(const char **ptr_ptr, const ObjData *data) {
  const char* ptr = *ptr_ptr;
  char *end = NULL;
  size_t index_count = 0;
  while (*ptr != '\n') {
    ObjIndices indices = {0};
    long int index = strtol(ptr, &end, 10);
    if (index < 0) {
      indices.f = data->verticies.v->size / 3 - (unsigned int)(-index);
    } else if (index > 0) {
      indices.f = (unsigned int)index - 1;
    }
    if (end == ptr) {
      return kErrorReadingObjFile;
    }
    ptr = end;
    if (*ptr == '/') {
      ++ptr;
      if (is_digit(*ptr)) {
        index = strtol(ptr, &end, 10);
        if (index < 0) {
          indices.t = data->verticies.t->size / 2 - (unsigned int)(-index);
        } else if (index > 0) {
          indices.t = (unsigned int)index - 1;
        }
        if (end == ptr) {
          return kErrorReadingObjFile;
        }
        ptr = end;
      }
    }
    if (*ptr == '/') {
      index = strtol(++ptr, &end, 10);
      if (index < 0) {
        indices.n = data->verticies.n->size / 3 - (unsigned int)(-index);
      } else if (index > 0) {
        indices.n = (unsigned int)index - 1;
      }
      if (end == ptr) {
        return kErrorReadingObjFile;
      }
      ptr = end;
    }
    if (index_count == 4) {
      return kErrorNotSupportedGeometry;
    }
    if (index_count == 3) {
      ObjIndices* indices_ptr = vector_push(data->indices, 2);
      if (indices_ptr == NULL) {
        return kErrorAllocationFailed;
      }
      *indices_ptr++ = *(ObjIndices*)vector_at(data->indices, data->indices->size - index_count);
      *indices_ptr = *(ObjIndices*)vector_at(data->indices, data->indices->size - 2);
    }
    ObjIndices* indices_ptr = vector_push(data->indices, 1);
    if (indices_ptr == NULL) {
      return kErrorAllocationFailed;
    }
    *indices_ptr = indices;
    ptr = skip_space(ptr);
    ++index_count;
  }
  *ptr_ptr = ptr;
  return kErrorNil;
}

static char *concat(const char *first, const char *second_p,
                    const char *second_e) {
  const size_t len_first = (first == NULL) ? 0 : strlen(first);
  const size_t len_second = second_e - second_p;
  char *str = (char *)malloc(len_first + len_second + 1);
  if (str == NULL) {
    return NULL;
  }
  if (first) {
    memcpy(str, first, len_first);
  }
  memcpy(str + len_first, second_p, len_second);
  str[len_first + len_second] = '\0';
  return str;
}

static inline const char *read_mtl_single(const char *ptr, float *mtl) {
  char *end = NULL;
  *mtl = strtof(ptr, &end);
  return end;
}

static inline const char *read_mtl_triple(const char *ptr, float triple[3]) {
  ptr = read_mtl_single(ptr, &triple[0]);
  ptr = read_mtl_single(ptr, &triple[1]);
  ptr = read_mtl_single(ptr, &triple[2]);

  return ptr;
}

static inline error read_map(const char **ptr_ptr, char **map_name) {
  const char* ptr = *ptr_ptr;
  ptr = skip_space(ptr);
  const char *end = end_of_name(ptr);
  *map_name = concat(NULL, ptr, end);
  if (*map_name) {
    return kErrorAllocationFailed;
  }
  *ptr_ptr = ptr;
  return kErrorNil;
}

static inline const char *read_mtl_int(const char *ptr, long int *val) {
  char *end = NULL;
  *val = strtol(ptr, &end, 10);
  return end;
}

static inline ObjNewMtl mtl_default() {
  return (ObjNewMtl){.Kd = {0.7, 0.7}, .Ni = 1.0f, .Ns = 32.0f, .d = 1.0f, .illum = 1};
}

static error mtl_open(const char *path, const ObjData *data) {
  char* buf = NULL;
  size_t count;
  error err = read_file(path, &buf, &count);
  const char *ptr = buf;
  const char *eof = ptr + count;

  bool found_d = false;
  ObjNewMtl mtl = mtl_default();

  while (ptr < eof) {
    ptr = skip_space(ptr);
    switch (*ptr) {
      case 'n':
        ++ptr;
        if (ptr[0] == 'e' &&
            ptr[1] == 'w' &&
            ptr[2] == 'm' &&
            ptr[3] == 't' &&
            ptr[4] == 'l' &&
            is_space(ptr[5])) {
          if (mtl.name) {
            ObjNewMtl* mtl_ptr = vector_push(data->mtl, 1);
            if (mtl_ptr == NULL) {
              err = kErrorAllocationFailed;
              goto cleanup;
            }
            *mtl_ptr = mtl;
            mtl = mtl_default();
          }

          ptr = skip_space(ptr + 5);
          const char *start = ptr;
          ptr = end_of_name(ptr);

          mtl.name = concat(NULL, start, ptr);
          if (mtl.name == NULL) {
            err = kErrorAllocationFailed;
            goto cleanup;
          }
        }
        break;
      case 'K':
        if (ptr[1] == 'a') {
          ptr = read_mtl_triple(ptr + 2, mtl.Ka);
        } else if (ptr[1] == 'd') {
          ptr = read_mtl_triple(ptr + 2, mtl.Kd);
        } else if (ptr[1] == 's') {
          ptr = read_mtl_triple(ptr + 2, mtl.Ks);
        } else if (ptr[1] == 'e') {
          ptr = read_mtl_triple(ptr + 2, mtl.Ke);
        }
        break;
      case 'N':
        if (ptr[1] == 's') {
          ptr = read_mtl_single(ptr + 2, &mtl.Ns);
        } else if (ptr[1] == 'i') {
          ptr = read_mtl_single(ptr + 2, &mtl.Ni);
        }
        break;
      case 'T':
        if (ptr[1] == 'r') {
          float Tr;
          ptr = read_mtl_single(ptr + 2, &Tr);
          if (!found_d) {
            mtl.d = 1.0f - Tr;
          }
        }
        break;
      case 'd':
        if (is_space(ptr[1])) {
          ptr = read_mtl_single(ptr + 1, &mtl.d);
          found_d = true;
        }
        break;
      case 'i':
        ++ptr;
        if (ptr[0] == 'l' &&
            ptr[1] == 'l' &&
            ptr[2] == 'u' &&
            ptr[3] == 'm' &&
            is_space(ptr[4])) {
          ptr = read_mtl_int(ptr + 4, &mtl.illum);
        }
        break;
      case 'm':
        ++ptr;
        if (ptr[0] == 'a' &&
            ptr[1] == 'p' &&
            ptr[2] == '_') {
          ptr += 3;
          if (*ptr == 'K') {
            ++ptr;
            if (ptr[0] == 'd' &&
                is_space(ptr[1])) {
              ++ptr;
              err = read_map(&ptr, &mtl.map_kd);
              if (err != kErrorNil) {
                goto cleanup;
              }
            }
          } else if (*ptr == 'N') {
            ++ptr;
            if (ptr[0] == 's' &&
                is_space(ptr[1])) {
              ++ptr;
              err = read_map(&ptr, &mtl.map_Ns);
              if (err != kErrorNil) {
                goto cleanup;
              }
            }
          } else if ((ptr[0] == 'b' || ptr[0] == 'B') &&
                      ptr[1] == 'u' &&
                      ptr[2] == 'm' &&
                      ptr[3] == 'p' &&
                      is_space(ptr[4])) {
            ptr += 4;
            err = read_map(&ptr, &mtl.map_bump);
            if (err != kErrorNil) {
              goto cleanup;
            }
          }
        }
        break;
      case '#':
      default:
        break;
    }
    ptr = skip_line(ptr);
  }
  if (mtl.name) {
    ObjNewMtl* mtl_ptr = vector_push(data->mtl, 1);
    if (mtl_ptr == NULL) {
      goto cleanup;
    }
    *mtl_ptr = mtl;
  }

cleanup:
  free(mtl.name);
  free(mtl.map_kd);
  free(mtl.map_Ns);
  free(mtl.map_bump);
  free(buf);

  return err;
}

static error parse_mtl(const char **ptr_ptr, const ObjData *data) {
  const char* ptr = *ptr_ptr;

  ptr = skip_space(ptr);
  const char *end = end_of_name(ptr);
  char *path_mtl = concat(data->dir_path, ptr, end);
  if (path_mtl == NULL) {
    return kErrorAllocationFailed;
  }
  mtl_open(path_mtl, data);
  free(path_mtl);

  *ptr_ptr = ptr;
  return kErrorNil;
}

static error parse_usemtl(const char** ptr_ptr, const ObjData *data) {
  const char* ptr = *ptr_ptr;
  ptr = skip_space(ptr);
  const char *end = end_of_name(ptr);
  char *name = concat(NULL, ptr, end);
  if (name == NULL) {
    return kErrorAllocationFailed;
  }
  for (size_t i = 0; i < data->mtl->size; ++i) {
    const ObjNewMtl* mtl_ptr = vector_at(data->mtl, i);
    if (!strcmp(mtl_ptr->name, name)) {
      ObjUseMtl* use_mtl_ptr = vector_push(data->usemtl, 1);
      if (use_mtl_ptr == NULL) {
        free(name);
        return kErrorAllocationFailed;
      }
      *use_mtl_ptr = (ObjUseMtl){
        .index = i
      };
      if (data->indices->size) {
        use_mtl_ptr = vector_at(data->usemtl, data->usemtl->size - 2);
        use_mtl_ptr->offset = data->indices->size;
      }
      break;
    }
  }
  free(name);
  *ptr_ptr = end;
  return kErrorNil;
}

static error parse_buffer(const char *ptr, const char *end, const ObjData *data) {
  error err;
  while (ptr != end) {
    ptr = skip_space(ptr);
    if (*ptr == 'v') {
      ++ptr;
      if (*ptr == ' ' || *ptr == '\t') {
        err = parse_position(&ptr, data->verticies.v, 3);
        if (err != kErrorNil) {
          return err;
        }
      } else if (*ptr == 'n') {
        err = parse_position(&ptr, data->verticies.n, 3);
        if (err != kErrorNil) {
          return err;
        }
      } else if (*ptr == 't') {
        err = parse_position(&ptr, data->verticies.t, 2);
        if (err != kErrorNil) {
          return err;
        }
      }
    } else if (*ptr == 'f') {
      ++ptr;
      if (*ptr == ' ' || *ptr == '\t') {
        err = parse_facet(&ptr, data);
        if (err != kErrorNil) {
          return err;
        }
      }
    } else if (*ptr == 'm') {
      ++ptr;
      if (ptr[0] == 't' &&
          ptr[1] == 'l' &&
          ptr[2] == 'l' &&
          ptr[3] == 'i' &&
          ptr[4] == 'b' &&
          is_space(ptr[5])) {
        ptr += 6;
        err = parse_mtl(&ptr, data);
        if (err != kErrorNil) {
          return err;
        }
      }
    } else if (*ptr == 'u') {
      ++ptr;
      if (ptr[0] == 's' &&
          ptr[1] == 'e' &&
          ptr[2] == 'm' &&
          ptr[3] == 't' &&
          ptr[4] == 'l' &&
          is_space(ptr[5])) {
        ptr += 6;
        err = parse_usemtl(&ptr, data);
        if (err != kErrorNil) {
          return err;
        }
      }
    }
    ptr = skip_line(ptr);
  }
  return kErrorNil;
}

error obj_data_parse(const char *path, ObjData* data) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    return kErrorReadingObjFile;
  } {
    const char *sep = strrchr(path, '/');
    data->dir_path = concat(NULL, path, ++sep);
    if (data->dir_path == NULL) {
      fclose(file);
      return kErrorAllocationFailed;
    }
  }
  error err = kErrorNil;
  char *buffer = (char *)malloc(2 * BUFFER_SIZE);
  if (buffer == NULL) {
    err = kErrorAllocationFailed;
    goto cleanup;
  }
  char* start = buffer;
  for (;;) {
    unsigned int read = fread(start, 1, BUFFER_SIZE, file);
    if (read == 0 && start == buffer) {
      break;
    }
    if (read < BUFFER_SIZE) {
      if (!read || start[read - 1] != '\n') {
        start[read++] = '\n';
      }
    }
    const char* end = start + read;
    if (end == buffer) {
      break;
    }
    const char *last = end;
    while (last > buffer) {
      last--;
      if (*last == '\n') {
        break;
      }
    }
    if (*last != '\n') {
      break;
    }
    ++last;
    err = parse_buffer(buffer, last, data);
    if (err != kErrorNil) {
      goto cleanup;
    }
    const size_t bytes = (size_t)(end - last);
    memmove(buffer, last, bytes);
    start = buffer + bytes;
  }
  if (data->mtl->size == 0) {
    ObjNewMtl* mtl_ptr = vector_push(data->mtl, 1);
    if (mtl_ptr == NULL) {
      err = kErrorAllocationFailed;
      goto cleanup;
    }
    *mtl_ptr = mtl_default();
  }
  if (data->usemtl->size == 0) {
    ObjUseMtl* use_mtl_ptr = vector_push(data->usemtl, 1);
    if (use_mtl_ptr == NULL) {
      err = kErrorAllocationFailed;
      goto cleanup;
    }
    *use_mtl_ptr = (ObjUseMtl){0};
  }
  {
    ObjUseMtl* use_mtl_ptr = vector_at(data->usemtl, data->usemtl->size - 1);
    use_mtl_ptr->offset = data->indices->size;
  }
cleanup:
  free(buffer);
  fclose(file);
  return err;
}


