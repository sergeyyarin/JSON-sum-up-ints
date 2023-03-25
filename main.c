#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <json-c/json.h>

static bool is_valid(const char * key, int32_t val)
{
    switch (val)
    {
    case 0 && errno == EINVAL:
        fprintf(stderr, "No conversion exists for key: %s\n", key);
        return false;
    case INT32_MAX:
        fprintf(stderr, "The value is too big for key: %s\n", key); 
        return false;
    case INT32_MIN:
        fprintf(stderr, "The value is too small for key: %s\n", key);
        return false;
    default:
        break;
    }
    return true;
}

static int add_obj_vals_sum_to_new_json(const json_object * obj, json_object * new)
{
    int64_t sum = 0;
    json_object_object_foreach(obj, k, v)
    {
        errno = 0;
        int32_t json_int_val = json_object_get_int(v);
        if (is_valid(k, json_int_val) == false)
        {
            continue;
        }
        json_object_object_add(new, k, v);
        /*
         * We have to increment refcount for value (v),
         * since it's ownership has been transferred to another object (new).
        */
        json_object_get(v);
        sum += json_int_val;
    }
    json_object * json_obj_sum = json_object_new_int64(sum);

    return json_object_object_add(new, "sum", json_obj_sum);
}

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        fprintf(stdout, "Usage: %s [input file] [output file]\n", argv[0]);
        exit(1);
    }

    json_object * obj_arr = json_object_from_file(argv[1]);
    if (!obj_arr)
    {
        fprintf(stderr, "Can't read file: %s\n", json_util_get_last_err());
        exit(1);
    }

    size_t obj_arr_len = json_object_array_length(obj_arr);

    json_object * obj_arr_new = json_object_new_array_ext(obj_arr_len);

    for (int obj_idx = 0; obj_idx < obj_arr_len; ++obj_idx)
    {
        json_object * obj = json_object_array_get_idx(obj_arr, obj_idx);
        if (!obj)
        {
            fprintf(stderr, "Null object at array index %d: %s\n", obj_idx, json_util_get_last_err());
            continue;   
        }

        json_object * obj_new = json_object_new_object();
        if (add_obj_vals_sum_to_new_json(obj, obj_new) < 0)
        {
            fprintf(stderr, "Failed trying to sum up the vals: %s\n", json_util_get_last_err());
            json_object_put(obj);
            json_object_put(obj_new);
            continue;
        }
        json_object_array_add(obj_arr_new, obj_new);
    }

    if (json_object_to_file(argv[2], obj_arr_new) < 0)
    {
        fprintf(stderr, "Failed writing json to file: %s\n", json_util_get_last_err());
        json_object_put(obj_arr_new);
        json_object_put(obj_arr);
        exit(1);
    }

    json_object_put(obj_arr_new);
    json_object_put(obj_arr);

    return 0;
}