#include <stdio.h>
#include <json-c/json.h>

static int add_obj_vals_sum_to_new_json(const json_object * obj, json_object * new)
{
    int sum = 0;
    json_object_object_foreach(obj, k, v)
    {
        if (json_object_get_type(v) == json_type_int)
        {
            sum += json_object_get_int(v);
        }
    }
    json_object * json_obj_int = json_object_new_int(sum);

    return json_object_object_add(new, k, json_obj_int);
}

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        fprintf(stdout, "Usage: %s [input file] [output file]", argv[0]);
        exit(1);
    }

    json_object * obj_arr = json_object_from_file(argv[1]);
    if (!obj_arr)
    {
        fprintf(stderr, "Can't read file: %s", json_util_get_last_err());
        exit(1);
    }

    size_t obj_arr_len = json_object_array_length(obj_arr);

    json_object * obj_arr_new = json_object_new_array_ext(obj_arr_len);

    for (int obj_idx = 0; obj_idx < obj_arr_len; ++obj_idx)
    {
        json_object * obj = json_object_array_get_idx(obj_arr, obj_idx);
        if (!obj)
        {
            fprintf(stderr, "Can't get array object by index: %s", json_util_get_last_err());
            exit(1);
        }

        json_object * obj_new = json_object_new_object();
        if (add_obj_vals_sum_to_new_json(obj, obj_new) < 0)
        {
            fprintf(stderr, "Failed trying to sum up the vals: %s\n", json_util_get_last_err());
            exit(1);
        }

        json_object_array_add(obj_arr_new, obj_new);
    }

    if (json_object_to_file(argv[2], obj_arr_new) < 0)
    {
        fprintf(stderr, "Failed writing json to file: %s\n", json_util_get_last_err());
        exit(1);
    }

    return 0;
}