<?php

/**
 * @file
 * Provide views data for file.module.
 */

use Drupal\field\FieldStorageConfigInterface;

/**
 * Implements hook_field_views_data().
 *
 * Views integration for file fields. Adds a file relationship to the default
 * field data.
 *
 * @see field_views_field_default_views_data()
 */
function file_field_views_data(FieldStorageConfigInterface $field_storage) {
  $data = field_views_field_default_views_data($field_storage);
  foreach ($data as $table_name => $table_data) {
    // Add the relationship only on the fid field.
    $data[$table_name][$field_storage->getName() . '_target_id']['relationship'] = array(
      'id' => 'standard',
      'base' => 'file_managed',
      'entity type' => 'file',
      'base field' => 'target_id',
      'label' => t('file from !field_name', array('!field_name' => $field_storage->getName())),
    );
  }

  return $data;
}

/**
 * Implements hook_field_views_data_views_data_alter().
 *
 * Views integration to provide reverse relationships on file fields.
 */
function file_field_views_data_views_data_alter(array &$data, FieldStorageConfigInterface $field_storage) {
  $entity_type_id = $field_storage->entity_type;
  $entity_manager = \Drupal::entityManager();
  $entity_type = $entity_manager->getDefinition($entity_type_id);
  $field_name = $field_storage->getName();
  $pseudo_field_name = 'reverse_' . $field_name . '_' . $entity_type_id;
  /** @var \Drupal\Core\Entity\Sql\DefaultTableMapping $table_mapping */
  $table_mapping = $entity_manager->getStorage($entity_type_id)->getTableMapping();

  list($label) = field_views_field_label($entity_type_id, $field_name);

  $data['file_managed'][$pseudo_field_name]['relationship'] = array(
    'title' => t('@entity using @field', array('@entity' => $entity_type->getLabel(), '@field' => $label)),
    'help' => t('Relate each @entity with a @field set to the file.', array('@entity' => $entity_type->getLabel(), '@field' => $label)),
    'id' => 'entity_reverse',
    'field_name' => $field_name,
    'entity_type' => $entity_type_id,
    'field table' => $table_mapping->getDedicatedDataTableName($field_storage),
    'field field' => $field_name . '_target_id',
    'base' => $entity_type->getBaseTable(),
    'base field' => $entity_type->getKey('id'),
    'label' => t('!field_name', array('!field_name' => $field_name)),
    'join_extra' => array(
      0 => array(
        'field' => 'deleted',
        'value' => 0,
        'numeric' => TRUE,
      ),
    ),
  );
}
