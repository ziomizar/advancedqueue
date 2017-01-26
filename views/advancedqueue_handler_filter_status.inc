<?php

/**
 * Filter by item status
 */
class advancedqueue_handler_filter_status extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Status');

      $this->value_options = array(
        ADVANCEDQUEUE_STATUS_QUEUED => t('Queued'),
        ADVANCEDQUEUE_STATUS_PROCESSING => t('Processing'),
        ADVANCEDQUEUE_STATUS_SUCCESS => t('Processed'),
        ADVANCEDQUEUE_STATUS_FAILURE => t('Failed'),
        ADVANCEDQUEUE_STATUS_FAILURE_RETRY => t('Retry'),
      );
    }
  }
}
