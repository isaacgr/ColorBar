typedef String (*FieldSetter)(String);
typedef String (*FieldGetter)();

const String NumberFieldType = "Number";
const String BooleanFieldType = "Boolean";
const String SelectFieldType = "Select";
const String ColorFieldType = "Color";
const String SectionFieldType = "Section";

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
  FieldGetter getModifiers;

} Field;

typedef Field FieldList[];
