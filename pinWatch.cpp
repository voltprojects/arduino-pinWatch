/**
* @author Rhys Bryant <code@voltprojects.com>
* basic library for monitoring pins and calling a given call back when a change Condition is matched  
* if you can see an issue or better way please report a bug
* @copyright Rhys Bryant <code@voltprojects.com>
*
* === Legal Jargen  ===
* This code is provided "AS IS" without warranty of any kind.
*   In no event shall the author be held liable for any damages arising from the
*   use of this code.
* @licence LGPL
* this notice MUST not be removed
*/

class PinWatch {
public:
	/**
	* @brief the Type class maps a type name to a callback for getting the value for that type
	**/
    class Type {
		public:
			char* name;
			pw_type_cb getValueFunc;
			Type(char* _name,pw_type_cb _callback) {
				name=_name;
				getValueFunc=_callback;
			}
			Type() {}
    };
	/**
	* @brief the Condition class defines the condition to be met 
	**/
    class Condition {
    public:
        char _operator;
        float value;
        Type* type;
        float lastValue;
		float currentValue;
		char index;
		/**
		 *  @brief checks to the given value meets the condition
		 *  @param currentValue the value to check
		*/
        bool match(float currentValue) {
            switch(_operator) {
            case OPT_RANGE://~
                return ((currentValue>lastValue && currentValue-lastValue>value) || (currentValue<lastValue &&  lastValue-currentValue>value));

                break;
            case OPT_GT://>
                return (currentValue>value);

                break;
            case OPT_LT://<
                return (currentValue<value);

                break;
            case OPT_EQ://=
                return (currentValue==value);
            default:
                return true;
            }
            return true;
        }
	private:
		static char tmp[40];
    public:
		int pin;
		/**
		 *  @param _type type object see type class
		 *  @param _pin the pin number to monitor
		 *  @param _op the operator to use when matching the condition eg >,<,= or ~
		 *  @param _value the value to use when matching the condition
		 *  for example the Condition Condition(myType,9,'=',1) would be matched when pin 9=1 assuming the Type mytype returned 1 or 0 for pin 9 
		 */
        Condition(Type* _type,int _pin,char _op,float _value) {
            pin=_pin;
            _operator=_op;
            value=_value;
            type=_type;
            lastValue=0;
			currentValue=0;
        }
        Condition() {}
        char* toString(){
			
			char line1=
			sprintf(tmp,"{\"id\":%d,\"pin\":%d,\"cv\":%s}"
			,index
			,pin
			,CliHelpers::getValue(currentValue)
			);
           return tmp;
        }
		/**
		 *  @brief checks to see if the current value matches the Condition
		 *  @return true if the value matches the Condition
		 */
        bool update() {
            float v=type->getValueFunc(pin);
            if(v!=lastValue) {
                currentValue=v;
				bool hasMatch=match(v);
				lastValue=v;
				
				return hasMatch;
            }
            else
                return false;
        }
    };
private:
    Condition conditions[CONDITION_COUNT];
    Type types[10];
    int conditionIndex;
    int typeIndex;
public:
    /**
     *  @brief finds a Type object by it's name
     *  
     *  @param [in] name the Type object's user friendly name
     *  @return the first Type object that has the given name 
     *  
     */
    Type* findTypeByName(char* name) {
        int size=sizeof(types)/sizeof(Type);
        for(int i=0; i<size; i++) {
            if(strcmp(name,types[i].name)==0)
                return &types[i];
        }
    }
    PinWatch() {
        conditionIndex=0;
        typeIndex=0;
    }
	/**
	 *  @param entry the ref to an object that was prevorly added using addWatchCondition
	 *  @return the internal index of the given condition after it was added using addWatchCondition
	 */
	int findMatch(Condition& entry){
		for(int i=0;i<conditionIndex;i++){
			if(entry._operator==conditions[i]._operator
			&& entry.type==conditions[i].type
			&& entry.value==conditions[i].value
			&& entry.pin==conditions[i].pin
			)
				return i;
		}
		return -1;
	}
	/**
	 *  @brief adds a Condition object to the inernal array for checking
	 *  @return the internal array index of the newly added object
	 */
    int addWatchCondition(Condition entry) {
		int i=findMatch(entry);
		if(i>-1)
			return i;
		if(CONDITION_COUNT<=conditionIndex)
			return -1;
			
		entry.index=conditionIndex;
        conditions[conditionIndex]=entry;
        conditionIndex++;
		return conditionIndex-1;
    }
	/**
	 *  @brief adds a Type object to the internal type array
	 *  @param the Type object to add
	 */
    void addType(Type type) {
        types[typeIndex]=type;
        typeIndex++;
    }
	/**
	 *  @brief checks each condition foreach match that is found callbackFunc is called with the Condition object
	 *  @param callbackFunc an callback function to call when a matching Condition is found
	 */
	void check(void (*callbackFunc)(PinWatch::Condition*)) {
        for(int i=0; i<conditionIndex; i++) {
            if(conditions[i].pin!=-1) {
                if(conditions[i].update()) {
                   callbackFunc(&conditions[i]);
                }
            }
        }

    }
};