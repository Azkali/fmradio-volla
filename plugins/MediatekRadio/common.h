/*
 * Copyright (C) 2014 The Android Open Source Project
 * Modifications copyright (C) 2026 Azkali Manad <a.ffcc7@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIATEKRADIO_COMMON_H
#define MEDIATEKRADIO_COMMON_H

#include "fm.h"

int COM_open_dev(const char *pname, int *fd);
int COM_close_dev(int fd);
int COM_pwr_up(int fd, int band, int freq, int deemphasis);
int COM_pwr_down(int fd, int type);
int COM_get_chip_id(int fd, int *chipid);
int COM_tune(int fd, int freq, int band);
int COM_set_mute(int fd, int mute);
int COM_pre_search(int fd);
int COM_restore_search(int fd);
int COM_Soft_Mute_Tune(int fd, fm_softmute_tune_t *para);
int COM_ana_switch(int fd, int antenna);
int COM_set_stereomono(int fd, int stemono);

#endif
