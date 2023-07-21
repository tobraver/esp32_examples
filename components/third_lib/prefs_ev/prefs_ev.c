#include "prefs_ev.h"
#include "esp_log.h"
#include "esp_log.h"

bool prefs_ev_read_idx(prefs_ev_t* hprefs_ev);
bool prefs_ev_write_idx(prefs_ev_t* hprefs_ev);

bool prefs_ev_init(prefs_ev_t* hprefs_ev)
{    
    if(!prefs_init(hprefs_ev->hprefs))
    {
        return false;
    }
    prefs_ev_read_idx(hprefs_ev);
    prefs_ev_write_idx(hprefs_ev);
    hprefs_ev->hmutex = mutex_create();
    return true;
}

bool prefs_ev_read_idx(prefs_ev_t* hprefs_ev)
{
    bool r = prefs_read_block(hprefs_ev->hprefs, hprefs_ev->hprefs.namespace, &hprefs_ev->idx, sizeof(hprefs_ev->idx));
    if(!r)
    {
        hprefs_ev->idx.read = 0;
        hprefs_ev->idx.write = 0;
    }
    return r;
}

bool prefs_ev_write_idx(prefs_ev_t* hprefs_ev)
{
    return prefs_write_block(hprefs_ev->hprefs, hprefs_ev->hprefs.namespace, &hprefs_ev->idx, sizeof(hprefs_ev->idx));
}

bool prefs_ev_config_idx(prefs_ev_t* hprefs_ev, prefs_idx_t idx)
{
    mutex_lock(hprefs_ev->hmutex);
    hprefs_ev->idx = idx;
    mutex_unlock(hprefs_ev->hmutex);

    return prefs_ev_write_idx(hprefs_ev);
}

bool prefs_ev_idx_read_inc(prefs_ev_t* hprefs_ev)
{
    mutex_lock(hprefs_ev->hmutex);
    if( hprefs_ev->idx.write < hprefs_ev->idx.read) 
    {
        hprefs_ev->idx.read++;
        hprefs_ev->idx.read%=hprefs_ev->max_count;
    }
    else 
    {
        hprefs_ev->idx.read++;
        if(hprefs_ev->idx.read >= hprefs_ev->idx.write)
        {
            hprefs_ev->idx.read = hprefs_ev->idx.write;
        }
    }
    mutex_unlock(hprefs_ev->hmutex);

    return prefs_ev_write_idx(hprefs_ev);
}

bool prefs_ev_idx_write_inc(prefs_ev_t* hprefs_ev)
{
    mutex_lock(hprefs_ev->hmutex);
    if( hprefs_ev->idx.write < hprefs_ev->idx.read )
    {
        if((hprefs_ev->idx.write+1) == hprefs_ev->idx.read) // full
        {
            hprefs_ev->idx.read++;
            hprefs_ev->idx.read%=hprefs_ev->max_count;
        }
        hprefs_ev->idx.write++;
        hprefs_ev->idx.write%=hprefs_ev->max_count;
    }
    else 
    {
        hprefs_ev->idx.write++;
        if(hprefs_ev->idx.write >= hprefs_ev->max_count)
        {
            hprefs_ev->idx.write = 0;
            hprefs_ev->idx.read++;
        }
    }
    mutex_unlock(hprefs_ev->hmutex);

    return prefs_ev_write_idx(hprefs_ev);
}

bool prefs_ev_is_empty(prefs_ev_t* hprefs_ev)
{
    if(hprefs_ev->idx.read == hprefs_ev->idx.write)  // empty
    {
        return true;
    }
    return false;
}

bool prefs_ev_peek_u64(prefs_ev_t* hprefs_ev, uint64_t* value)
{
    ESP_LOGI("prefs_ev_peek_u64", "read: %d, write: %d", hprefs_ev->idx.read, hprefs_ev->idx.write);
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return false;
    }

    char key[16] = {0};
    utoa(hprefs_ev->idx.read, key, 16);
    const char* TAG = "prefs_ev_peek_u64";
    ESP_LOGI(TAG, "key: %s", key);

    return prefs_read_u64(hprefs_ev->hprefs, key, value);
}

bool prefs_ev_peek_u64_muti(prefs_ev_t* hprefs_ev, uint32_t idx, uint32_t cnt, uint64_t value[], uint32_t* length)
{
    ESP_LOGI("prefs_ev_peek_u64_muti", "read: %d, write: %d", hprefs_ev->idx.read, hprefs_ev->idx.write);
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return false;
    }

    prefs_idx_t conf_idx = hprefs_ev->idx; // current

    for(uint32_t i=0; i<idx; i++)
    {
        prefs_ev_move(hprefs_ev);
    }

    for(uint32_t i=0; i<cnt; i++)
    {
        if(prefs_ev_peek_u64(hprefs_ev, &value[i]) == false)
        {
            break;
        }
        *length = i+1;
        prefs_ev_move(hprefs_ev);
    }

    prefs_ev_config_idx(hprefs_ev, conf_idx); // recover
    return *length ? true : false;
}

bool prefs_ev_read_u64(prefs_ev_t* hprefs_ev, uint64_t* value)
{
    ESP_LOGI("prefs_ev_read_u64", "read: %d, write: %d", hprefs_ev->idx.read, hprefs_ev->idx.write);
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return false;
    }

    char key[16] = {0};
    utoa(hprefs_ev->idx.read, key, 16);
    const char* TAG = "prefs_ev_read_u64";
    ESP_LOGI(TAG, "key: %s", key);

    if(prefs_read_u64(hprefs_ev->hprefs, key, value))
    {
        if(prefs_erase_key(hprefs_ev->hprefs, key))
        {
            prefs_ev_idx_read_inc(hprefs_ev);
            return true;
        }
    }
    return false;
}

bool prefs_ev_write_u64(prefs_ev_t* hprefs_ev, uint64_t value)
{
    ESP_LOGI("prefs_ev_write_u64", "read: %d, write: %d", hprefs_ev->idx.read, hprefs_ev->idx.write);
    char key[16] = {0};
    utoa(hprefs_ev->idx.write, key, 16);
    const char* TAG = "prefs_ev_write_u64";
    ESP_LOGI(TAG, "key: %s", key);

    if(prefs_write_u64(hprefs_ev->hprefs, key, value))
    {
        prefs_ev_idx_write_inc(hprefs_ev);
        return true;
    }
    return false;
}

bool prefs_ev_peek_block(prefs_ev_t* hprefs_ev, void* buff, uint32_t size)
{
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return false;
    }

    char key[16] = {0};
    utoa(hprefs_ev->idx.read, key, 16);
    const char* TAG = "prefs_ev_peek_block";
    ESP_LOGI(TAG, "key: %s", key);

    return prefs_read_block(hprefs_ev->hprefs, key, buff, size);
}

bool prefs_ev_peek_block_muti(prefs_ev_t* hprefs_ev, uint32_t idx, uint32_t cnt, void* value, uint32_t size, uint32_t* length)
{
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return false;
    }

    prefs_idx_t conf_idx = hprefs_ev->idx; // current
    uint8_t* pvalue = (uint8_t*)value;

    for(uint32_t i=0; i<idx; i++)
    {
        prefs_ev_move(hprefs_ev);
    }

    for(uint32_t i=0; i<cnt; i++)
    {
        if(prefs_ev_peek_block(hprefs_ev, pvalue, size) == false)
        {
            break;
        }
        *length = i+1;
        pvalue += size;
        prefs_ev_move(hprefs_ev);
    }

    prefs_ev_config_idx(hprefs_ev, conf_idx); // recover
    return *length ? true : false;
}

bool prefs_ev_read_block(prefs_ev_t* hprefs_ev, void* buff, uint32_t size)
{
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return false;
    }

    char key[16] = {0};
    utoa(hprefs_ev->idx.read, key, 16);

    if(prefs_read_block(hprefs_ev->hprefs, key, buff, size))
    {
        if(prefs_erase_key(hprefs_ev->hprefs, key))
        {
            prefs_ev_idx_read_inc(hprefs_ev);
            return true;
        }
    }
    return false;
}

bool prefs_ev_write_block(prefs_ev_t* hprefs_ev, void* buff, uint32_t size)
{
    char key[16] = {0};
    utoa(hprefs_ev->idx.write, key, 16);
    const char* TAG = "prefs_ev_write_block";
    ESP_LOGI(TAG, "key: %s", key);

    if(prefs_write_block(hprefs_ev->hprefs, key, buff, size))
    {
        prefs_ev_idx_write_inc(hprefs_ev);
        return true;
    }
    return false;
}

bool prefs_ev_move(prefs_ev_t* hprefs_ev)
{
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return true;
    }

    prefs_ev_idx_read_inc(hprefs_ev);
    return true;
}

bool prefs_ev_remove(prefs_ev_t* hprefs_ev)
{
    if(prefs_ev_is_empty(hprefs_ev))
    {
        return true;
    }

    char key[16] = {0};
    utoa(hprefs_ev->idx.read, key, 16);

    if(prefs_erase_key(hprefs_ev->hprefs, key))
    {
        prefs_ev_idx_read_inc(hprefs_ev);
        return true;
    }
    return false;
}

bool prefs_ev_clear(prefs_ev_t* hprefs_ev)
{
    return prefs_erase_namespace(hprefs_ev->hprefs);
}





