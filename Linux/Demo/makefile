#############################################################################
#
#  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
#
#  This file is part of the FreeRTOS Add-ons project.
#
#  Source Code:
#  https://github.com/michaelbecker/freertos-addons
#
#  Project Page:
#  http://michaelbecker.github.io/freertos-addons/
#
#  On-line Documentation:
#  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so,subject to the following conditions:
#
#  + The above copyright notice and this permission notice shall be included
#    in all copies or substantial portions of the Software.
#  + Credit is appreciated, but not required, if you find this project useful
#    enough to include in your application, product, device, etc.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#
#############################################################################

CORES ?= $(shell nproc)
MAKEFLAGS+="-j $(CORES)"

SUBDIRS = 	Linux_g++_binary_semaphore \
	Linux_g++_binary_semaphore_no_except \
	Linux_gcc_mem_pools \
	Linux_gcc_mem_pools_add_extra \
	Linux_gcc_mem_pools_static \
	Linux_gcc_read_write_lock_prefer_reader \
	Linux_gcc_read_write_lock_prefer_writer \
	Linux_gcc_simple_tasks \
	Linux_gcc_workqueues \
	Linux_gcc_workqueues_no_delete \
	Linux_gcc_zero_copy_queue \
	Linux_g++_condition_variables \
	Linux_g++_condition_variables2 \
	Linux_g++_condition_variables_multiple_producers_consumers \
	Linux_g++_condition_variables_multiple_producers_consumers2 \
	Linux_g++_counting_semaphore \
	Linux_g++_counting_semaphore_no_except \
	Linux_g++_critical_section \
	Linux_g++_delay_until \
	Linux_g++_dynamic_tasks \
	Linux_g++_dynamic_tasks_high_pri \
	Linux_g++_dynamic_tasks_multistart_scheduler_off \
	Linux_g++_dynamic_tasks_multistart_scheduler_on \
	Linux_g++_mem_pools \
	Linux_g++_mem_pools_add \
	Linux_g++_mem_pools_static \
	Linux_g++_mutex_recursive \
	Linux_g++_mutex_recursive_no_except \
	Linux_g++_mutex_standard \
	Linux_g++_mutex_standard_no_except \
	Linux_g++_queues_multiple_producers \
	Linux_g++_queues_multiple_producers_multiple_consumers \
	Linux_g++_queues_multiple_producers_multiple_consumers_no_except \
	Linux_g++_queues_multiple_producers_no_except \
	Linux_g++_queues_simple_producer_consumer \
	Linux_g++_queues_simple_producer_consumer_no_except \
	Linux_g++_read_write_lock_prefer_reader \
	Linux_g++_read_write_lock_prefer_reader_no_except \
	Linux_g++_read_write_lock_prefer_writer \
	Linux_g++_read_write_lock_prefer_writer_no_except \
	Linux_g++_simple_tasks \
	Linux_g++_simple_tasks_no_cpp_strings \
	Linux_g++_simple_tasks_no_vTaskDelete \
	Linux_g++_task_delete \
	Linux_g++_tasklet_dtor \
	Linux_g++_tasklet_dtor_no_except \
	Linux_g++_tasklets \
	Linux_g++_tasklets_no_except \
	Linux_g++_tickhook_disable \
	Linux_g++_tickhooks \
	Linux_g++_timers \
	Linux_g++_timers_no_except \
	Linux_g++_unnamed_tasks \
	Linux_g++_unnamed_tasks_no_cpp_strings \
	Linux_g++_workqueues \
	Linux_g++_workqueues_delete \

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
