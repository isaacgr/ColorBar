typedef String (*FieldSetter)(String);
typedef String (*FieldGetter)();
typedef void (*FieldValueSetter)(uint8_t);
typedef uint8_t (*FieldIndexGetter)();

const String NumberFieldType = "Number";
const String SelectFieldType = "Select";
const String ColorFieldType = "Color";

typedef struct
{
public:
  String name;
  String label;
  String type;
  bool modifier;
  uint8_t min;
  uint8_t max;
  FieldGetter getValue;
  FieldGetter getOptions;
  FieldSetter setValue;
  FieldValueSetter setByValue;
  FieldIndexGetter getValueIndex;

} Field;

typedef Field FieldList[];
