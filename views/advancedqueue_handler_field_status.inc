<?php

/**
 * Field handler for item status.
 */
class advancedqueue_handler_field_status extends views_handler_field {
  function render($values) {
    $options = array(
      -1 => t('Queued'),
      0 => t('Processing'),
      1 => t('Processed'),
      2 => t('Failed'),
    );
    $classes = array(
      -1 => 'queued',
      0 => 'processing',
      1 => 'processed',
      2 => 'failed',
    );

    $output = array(
      '#attached' => array(
        'css' => array(drupal_get_path('module', 'advancedqueue') . '/advancedqueue.css'),
      ),
      '#markup' => '<span class="' . check_plain($values->{$this->field_alias}) . ' advancedqueue-status-' . $classes[$values->{$this->field_alias}] . '">' . $options[$values->{$this->field_alias}] . '</span>',
    );

    return drupal_render($output);
  }
}
