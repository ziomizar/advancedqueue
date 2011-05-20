<?php

/**
 * @file
 * Drush worker for Advanced queues.
 */

/**
 * Implementation of hook_drush_command().
 */
function advancedqueue_drush_command() {
  $items = array();
  $items['advancedqueue-process-queue'] = array(
    'description' => 'Run a processing job for a queue.',
    'arguments' => array(
      'queue' => dt('The name of the queue to process.'),
    ),
    'options' => array(
      'timeout' => 'The maximum execution time of the script.',
      'all' => 'Process all queues.',
    ),
  );
  return $items;
}

function drush_advancedqueue_process_queue() {
  // Load information about the registred queues, and sort them by weight.
  $all_queue_info = module_invoke_all('advanced_queue_info');
  drupal_alter('advanced_queue_info', $all_queue_info);
  uasort($all_queue_info, 'drupal_sort_weight');

  $all_option = (bool) drush_get_option('all');
  $queues = func_get_args();
  if (!($all_option xor !empty($queues))) {
    return drush_set_error(dt('You have to specify either a set of queues or the --all parameter.'));
  }

  if ($all_option) {
    $queues = $all_queue_info;
  }
  else {
    // Validate queues.
    $queues = array_combine($queues, $queues);
    $invalid_queues = array_diff_key($queues, $all_queue_info);
    if ($invalid_queues) {
      return drush_set_error(dt('The following queues are invalid: !queues. Aborting.', array('!queues' => implode(', ', $invalid_queues))));
    }
    $queues = array_intersect_key($all_queue_info, $queues);
  }

  // Run the worker for a certain period of time before killing it.
  $timeout = drush_get_option('timeout');
  $end = $timeout ? time() + $timeout : 0;

  drush_log(dt('Starting processing loop.'));

  while (!$end || time() < $end) {
    foreach ($queues as $queue_name => $queue_info) {
      $queue = DrupalQueue::get($queue_name);

      if ($item = $queue->claimItem()) {
        drush_advancedqueue_process_item($queue, $queue_name, $queue_info, $item);
        continue 2;
      }
    }

    // Not item processed in that round, let the CPU rest.
    sleep(1);
  }

  drush_log(dt('Timeout: exiting processing loop.'));
}

function drush_advancedqueue_process_item($queue, $queue_name, $queue_info, $item) {
  $function = $queue_info['worker callback'];
  drush_log(dt('[!queue:!id] Starting processing item (!item).', array('!queue' => $queue_name, '!id' => $item->item_id, '!item' => json_encode($item->data))));

  try {
    $output = $function($item->data);
    if (is_array($output)) {
      $item->status = $output[0];
      $item->result = $output[1];
    }
    else {
      // TODO: remove magic constants.
      $item->status = $output ? 1 : 2;
    }
  }
  catch (Exception $e) {
    drush_log(dt('[!queue:!id] failed processing: !message', array('!queue' => $queue_name, '!id' => $item->item_id, '!message' => (string) $e)));
    // TODO: remove magic constants.
    $item->status = 2;
  }

  drush_log(dt('[!queue:!id] Processing ended.', array('!queue' => $queue_name, '!id' => $item->item_id, '!item' => json_encode($item->data))));
  $queue->deleteItem($item);
}
