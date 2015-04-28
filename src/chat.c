#include "chat.h"
#include "counter.h"

void status_err(){
//  cdc_write_buf(&cdc_out, "\nERR\n", 5);
  return;
}

void status_ok(){
//  cdc_write_buf(&cdc_out, "\nOK\n", 4);
  return;
}


void vChatTask(void *vpars){
  char s[64];
  char cmd[64];
  char *tk;
  int16_t i;

  while (1){
    cdc_gets(cmd, sizeof(cmd));
//    cdc_write_buf(&cdc_out, cmd, strlen(cmd),1);

    tk = _strtok(cmd, " \n");
      if (strcmp(tk, "get")==0){
        xSemaphoreTake(adc_vals_lock, portMAX_DELAY);
        for (i=0; i<ADC_NCHAN; i++){
          sniprintf(s,sizeof(s),"%6d", adc_vals[i]);
          cdc_write_buf(&cdc_out, s, strlen(s),1);
        }
        cdc_write_buf(&cdc_out, "\n",1,1);

        xSemaphoreGive(adc_vals_lock);
      }
      else if (strcmp(tk, "cnt")==0) {
        for (i = 0; i < CNT_NUM; i++) {
          sniprintf(s,sizeof(s),"%d ", cnt_get(i));
          cdc_write_buf(&cdc_out, s, strlen(s),1);
        }
        cdc_write_buf(&cdc_out, "\n",1,1);
      } else {
        cdc_write_buf(&cdc_out, "ERROR: unknown command\n", 0,1);
      }
        
      }
}

