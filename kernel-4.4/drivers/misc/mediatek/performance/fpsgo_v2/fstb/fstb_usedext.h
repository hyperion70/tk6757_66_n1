/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FSTB_USEDEXT_H
#define FSTB_USEDEXT_H

#include <fpsgo_common.h>
#include <trace/events/fpsgo.h>
#include <linux/list.h>
#include <linux/sched.h>


extern int (*fbt_notifier_cpu_frame_time_fps_stabilizer)(
	int pid,
	int frame_type,
	unsigned long long Q2Q_time,
	unsigned long long Runnging_time,
	unsigned int Curr_cap,
	unsigned int Max_cap,
	unsigned int Target_fps);
extern void (*display_time_fps_stablizer)(unsigned long long ts);
extern void fbt_cpu_vag_set_fps(unsigned int fps);

enum {
	NON_VSYNC_ALIGNED_TYPE = 0,
	VSYNC_ALIGNED_TYPE = 1,
	BY_PASS_TYPE = 2,
};



#define FRAME_TIME_BUFFER_SIZE 300
struct FSTB_FRAME_INFO {
	struct hlist_node hlist;

	int pid;
	int target_fps;
	int queue_fps;
	int frame_type;
	int render_method;
	int asfc_flag;
	int new_info;

	unsigned long long queue_time_ts[FRAME_TIME_BUFFER_SIZE]; /*timestamp*/
	int queue_time_begin;
	int queue_time_end;
	long long q2q_time[FRAME_TIME_BUFFER_SIZE];
	long long weighted_cpu_time[FRAME_TIME_BUFFER_SIZE];
	long long weighted_cpu_time_ts[FRAME_TIME_BUFFER_SIZE];
	int cur_capacity[FRAME_TIME_BUFFER_SIZE];
	int weighted_cpu_time_begin;
	int weighted_cpu_time_end;
	long long sorted_weighted_cpu_time[FRAME_TIME_BUFFER_SIZE];

};

#endif

