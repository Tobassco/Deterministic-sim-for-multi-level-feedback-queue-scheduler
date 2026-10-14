# Deterministic-sim-for-multi-level-feedback-queue-scheduler

As part of COMP30023 we were tasked with understanding CPU scheduling by writing a deterministic simulator for a Multi-Level Feedback Queue (MLFQ) scheduler. The simulator runs in discrete time units and schedules processes on a single CPU (one process at a time).
I progressively built the scheduler from a single-queue round-robin design to a multi-queue MLFQ scheduler where processes arrive at the highest-priority queue, are demoted to a lower- level queue after consuming their quantum, and are subject to priority boosting as a starvation mitigation strategy.
