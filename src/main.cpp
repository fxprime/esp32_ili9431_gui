#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <lv_examples.h>

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{

    Serial.printf("%s@%d->%s\r\n", file, line, dsc);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch(&touchX, &touchY, 600);

    if(!touched) {
      data->state = LV_INDEV_STATE_REL;
    } else {
      data->state = LV_INDEV_STATE_PR;
	    
      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;
  
      // Serial.print("Data x");
      // Serial.println(touchX);
      
      // Serial.print("Data y");
      // Serial.println(touchY);
    }

    return false; /*Return `false` because we are not buffering and no more data to read*/
}












static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked\n");
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Toggled\n");
    }
}

void lv_ex_btn_3(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, event_handler);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn2, event_handler);
    lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_btn_set_checkable(btn2, true);
    lv_btn_toggle(btn2);
    lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Toggled");
}












static lv_obj_t * kb;
static lv_obj_t * ta;
static lv_obj_t *tabview;

static void kb_event_cb(lv_obj_t * keyboard, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);
    if(e == LV_EVENT_CANCEL) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_del(kb);
        kb = NULL;
    }
}

static void kb_create(void)
{
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_keyboard_set_cursor_manage(kb, true);
    lv_obj_set_event_cb(kb, kb_event_cb);
    lv_keyboard_set_textarea(kb, ta);

}

static void kb_delete(void)
{
    lv_keyboard_set_textarea(kb, NULL); /*De-assign the text area  to hide it cursor if needed*/
    lv_obj_del(kb);
}

static void ta_event_cb(lv_obj_t * ta_local, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED && kb == NULL) {
      Serial.println("Create");
        kb_create();
    }
}

void lv_ex_keyboard_3(void)
{

    /*Create a text area. The keyboard will write here*/
    ta  = lv_textarea_create(lv_scr_act(), NULL);
    // lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 16);

    lv_obj_set_pos(ta, 10, 200);

    lv_obj_set_event_cb(ta, ta_event_cb);
    lv_textarea_set_text(ta, "");
    lv_coord_t max_h = LV_VER_RES / 2 - LV_DPI / 8;
    if(lv_obj_get_height(ta) > max_h) lv_obj_set_height(ta, max_h);

    kb_create();
}












static void tab_event(lv_obj_t * obj, lv_event_t event) {
  if( event == LV_EVENT_VALUE_CHANGED) {
    Serial.println("Page changed"); 
    if(lv_tabview_get_tab_act(tabview)!=1 && kb != NULL) {
      // kb_delete();
      lv_keyboard_set_textarea(kb, NULL); /*De-assign the text area  to hide it cursor if needed*/
       lv_obj_del(kb);
       kb = NULL;
       Serial.println("delete");
    }
  }
}








lv_chart_series_t * ser1;

lv_obj_t * chart;



void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    lv_init();

#if USE_LV_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    tft.begin(); /* TFT init */
    tft.setRotation(2); /* Landscape orientation */

  uint16_t calData[5] = { 351, 3395, 305, 3563, 2 };
    tft.setTouch(calData);
  
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 320;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

		/* Try an example from the lv_examples repository
		 * https://github.com/lvgl/lv_examples*/

    //  lv_ex_tabview_1();
		// lv_ex_btn_3(); 
    // lv_ex_keyboard_3();
    // lv_demo_widgets();
    // lv_ex_tileview_1();
    // lv_ex_get_started_2();


    /*Create a Tab view object*/
    
    tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_tabview_set_anim_time(tabview, 200);

    
    lv_obj_set_event_cb(tabview, tab_event);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Tab 3");
    
 
 
    lv_obj_t * label;
    
    lv_obj_t * btn1 = lv_btn_create(tab1, NULL);
    lv_obj_set_event_cb(btn1, event_handler);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);
  
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");








    /*Create a text area. The keyboard will write here*/
    ta  = lv_textarea_create(tab2, NULL);
    // lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 16);

    lv_obj_set_pos(ta, 10, 100);

    lv_obj_set_event_cb(ta, ta_event_cb);
    lv_textarea_set_text(ta, "");
    lv_coord_t max_h = LV_VER_RES / 2 - LV_DPI / 8;
    if(lv_obj_get_height(ta) > max_h) lv_obj_set_height(ta, max_h);









        /*Create a chart*/
    chart = lv_chart_create(tab3, NULL);
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add a faded are effect*/
    lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/


    /*Add two data series*/
    ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, LV_COLOR_GREEN);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next(chart, ser1, 31);
    lv_chart_set_next(chart, ser1, 66);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 89);
    lv_chart_set_next(chart, ser1, 63);
    lv_chart_set_next(chart, ser1, 56);
    lv_chart_set_next(chart, ser1, 32);
    lv_chart_set_next(chart, ser1, 35);
    lv_chart_set_next(chart, ser1, 57);
    lv_chart_set_next(chart, ser1, 85);

    /*Directly set points on 'ser2'*/
    ser2->points[0] = 92;
    ser2->points[1] = 71;
    ser2->points[2] = 61;
    ser2->points[3] = 15;
    ser2->points[4] = 21;
    ser2->points[5] = 35;
    ser2->points[6] = 35;
    ser2->points[7] = 58;
    ser2->points[8] = 31;
    ser2->points[9] = 53;

    lv_chart_refresh(chart); /*Required after direct set*/
 


}


void loop()
{

    lv_task_handler(); /* let the GUI do its work */

    time_t t_now = millis();
    static time_t last_update = 0;
    if(t_now - last_update > 5) {
      last_update = t_now;
      lv_chart_set_next(chart, ser1, 10*sin( (2*PI*millis()/1000.0)/0.2 )+50 );
    }
    
    delay(5);
}
