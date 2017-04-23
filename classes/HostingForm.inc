<?php
/**
 * @file The HostingForm class.
 */

abstract class HostingForm {

  // The form's definition, passed by reference.
  public $form = FALSE;

  // The form's state, passed by reference.
  public $form_state = FALSE;

  // The node associated with the form, if available.
  public $node = FALSE;

  /**
   * Contructor method.
   *
   * Classes based on HostingForm should be instantiated by passing in $form
   * and $form_state, such as from hook_form_alter(), validate or submit
   * callbacks. These get passed by reference, so that any changes made need
   * not be returned back to the calling function.
   */
  public function __construct(&$form, &$form_state, $node = FALSE) {
    $this->form =& $form;
    $this->form_state =& $form_state;
    if ($node) {
      $this->node = $node;
    }
    elseif (isset($form['#node'])) {
      $this->node = $form['#node'];
    }
    elseif (isset($form_state['build_info']['args'][0])) {
      $this->node = $form_state['build_info']['args'][0];
    }
  }

  /**
   * Properly close dialogs, or re-direct back to source node, depending on
   * whether overlay.module is enabled.
   *
   * TODO: Move something like this upstream (into hosting_task.module)?
   */
  function closeOverlay() {
    if (module_exists('overlay') && overlay_get_mode() == 'child') {
      overlay_close_dialog();
    }
    else {
      drupal_goto('node/' . $this->node->nid);
    }
    $this->form_state['rebuild'] = TRUE;
  }

  /**
   * Render a form field un-editable.
   */
  public function makeFieldReadOnly($field) {
    $this->form[$field]['#disabled'] = TRUE;
  }

  /**
   * Hide a form field.
   */
  protected function disableField($field) {
    $this->form[$field]['#access'] = FALSE;
  }

  /**
   * Determine whether we're creating a new node, or editing an existing one.
   */
  protected function isANewNode() {
    return isset($this->form['nid']) && is_null($this->form['nid']['#value']);
  }

  protected function language() {
    return $this->form['language']['#value'];
  }

  protected function setFieldListOptions($field, $options) {
    $this->form[$field][$this->language()]['#options'] = $options;
  }

  /**
   * Register a form validate handler.
   */
  public function addValidateHandler($callback) {
    $this->form['#validate'][] = $callback;
  }

  public function error($field, $message) {
    form_error($this->form[$field], t($message));
  }

  public function getFormStateValue($field, $index = 0) {
    return $this->form_state['values'][$field][$this->language()][$index]['value'];
  }

  public function setFormStateValue($field, $value, $index = 0) {
    $this->form_state['values'][$field][$this->language()][$index]['value'] = $value;
  }

}
