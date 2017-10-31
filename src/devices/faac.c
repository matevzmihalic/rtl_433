/* FAAC garage door opener
 *
 * Tested with FAAC TML2-433-SLR
 * 
 * I have no idea what that data means or if this is correct encoding,
 * but since I only need the raw data it's good enough.
 */


#include "rtl_433.h"
#include "pulse_demod.h"
#include "util.h"

#define FAAC_SHORT_LIMIT 200

static int faac_callback(bitbuffer_t *bitbuffer) {
    char time_str[LOCAL_TIME_BUFLEN];
    uint8_t *bb;
    uint16_t brow;
    data_t *data;
    int valid = 0;

    local_time_str(0, time_str);

    for (brow = 0; brow < bitbuffer->num_rows; ++brow) {
        bb = bitbuffer->bb[brow];

        // check length
        if (bitbuffer->bits_per_row[brow] != 107)
            continue;

        // check start and stop sequence
        if ((bb[0] & 0xf0) != 0xf0 || bb[13] != 0)
            continue;

            if (debug_output > 1) {
                fprintf(stderr,"new_tmplate callback:\n");
        
                bitbuffer_print(bitbuffer);
            }

            char str[27];
            sprintf(str, "%x%x%x%x%x%x%x%x%x%x%x%x%x", bb[0], bb[1], bb[2], bb[3], bb[4], bb[5], bb[6], bb[7], bb[8], bb[9], bb[10], bb[11], bb[12]);

            data = data_make(
                "time", "", DATA_STRING, time_str,
                "model", "", DATA_STRING, "FAAC",
                "data","", DATA_STRING, str,
                NULL);

        data_acquired_handler(data);

        valid++;
    }

    // Return 1 if message successfully decoded
    if (valid)
        return 1;

    return 0;
}

static char *csv_output_fields[] = {
    "time",
    "model",
    "data",
    NULL
};


r_device faac = {
    .name          = "FAAC",
    .modulation    = FSK_PULSE_PCM,
    .short_limit   = 4*FAAC_SHORT_LIMIT,
    .long_limit    = 4*FAAC_SHORT_LIMIT,
    .reset_limit   = 4*FAAC_SHORT_LIMIT*3,
    .json_callback = &faac_callback,
    .disabled      = 0,
    .fields        = csv_output_fields,
};
