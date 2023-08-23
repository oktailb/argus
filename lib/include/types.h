#pragma once

/**
 * @struct s_argusExchange
 * @brief simple data map to handle the Argus SHM data exchanges
 * @param width
 * @param height
 * @param size
 * @param inWrite kind of basic mutex to avoid image flickering due to re-writing avec acquisition
 */
typedef struct s_argusExchange {
    int width;
    int height;
    int size;
    bool inWrite;
} t_argusExchange;
