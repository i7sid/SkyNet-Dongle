<?php
/**
 * IGC_I_Record class extends IGC_Record. The I record defines the extension
 * of the mandatory fix B Record. Only one I record is allowed in each file.
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_I_Record extends IGC_Record
{
  /**
   * Numbers of extensions in B records
   *
   * @access public
   * @var integer
   */
  public $number_of_extensions;

  /**
   * Extensions and the positions of the extensions in the B records
   *
   * @access public
   * @var array
   */
  public $extensions;

  /**
   * Class constructor creates the I record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'I';
    $this->raw = $record;

    $this->number_of_extensions = intval(substr($record, 1, 2));
    $this->extensions = array();
    for ($i = 0; $i < $this->number_of_extensions; $i++) {
      $extension_code = substr($record, 1 + 2 + $i * 7 + 4, 3);
      $this->extensions[$extension_code] =
        array("start_byte" => intval(substr($record, 1 + 2 + $i * 7, 2)),
              "finish_byte" => intval(substr($record, 1 + 2 + $i * 7 + 2, 2)));
    }
    IGC_B_Record::SetExtensionOffsets($this->extensions);
  }
}
?>
