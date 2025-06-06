import {defineComponent, isProxy, provide, reactive, ref} from "vue";
import {getMetaValue, initMetaValue, setMetaValue, removeMeta} from "@/utils/MetaUtils";
import {FormContext} from "@/components/form/basic/FormContext";
import SysUtils from "@/utils/SysUtils";


export default defineComponent({
    name: 'UForm',
    props: {
        span: Array, // labelCol wrapperCol eg: [3, 21]
        // 和AForm不同的是model必须双向绑定的方式即：v-model="model"
        'onUpdate:modelValue': {type: Function},
        modelValue: {type: Object, default: () => { return {}}}
    },
    setup({modelValue}, {attrs}) {
        if(attrs.model) {
            console.warn("UForm不支持[:model] 请使用[v-model]替代")
        }

        let formRef;
        let initModel = modelValue;
        let editModel = ref(initModel);
        let isFormMount = ref(false);
        let formContext = new FormContext();
        provide('initModel', (namePath, value) => {
            // 如果form已经挂在完成, 其余的表单将不在进行初始化到[initModel]
            if(!isFormMount.value) {
                initMetaValue(namePath, initModel, value)
            } else { // 直接放入editModel
                let newValue = getMetaValue(namePath, editModel.value);
                if(newValue == undefined) { // 如果值不存在直接做初始化, 如果值存在不能将默认值覆盖掉真实值
                    setMetaValue(namePath, editModel.value, value);
                }
            }
        })
        provide('formContext', formContext);

        formContext['removeField'] = (namePath) => removeMeta(namePath, editModel.value);
        formContext['getFieldValue'] = (namePath) => getMetaValue(namePath, editModel.value);
        formContext['initFieldValue'] = (namePath, value) => initMetaValue(namePath, initModel, value);
        formContext['setFieldValue'] = (namePath, value) => setMetaValue(namePath, editModel.value, value);

        return {formContext, formRef, initModel, editModel, isFormMount}
    },
    created() {
        this.formRef = this.$refs['formRef']

        this.formContext.getRules = this.getRules;
        this.formContext.validate = this.validate;
        this.formContext.resetFields = this.resetFields;
        this.formContext.getEditModel = this.getEditModel;
        this.formContext.setEditModel = this.setEditModel;
        this.formContext.getInitModel = this.getInitModel;
        this.formContext.scrollToField = this.scrollToField;
        this.formContext.clearValidate = this.clearValidate;
        this.formContext.validateFields = this.validateFields;
    },
    mounted() {
        this.isFormMount = true;
        // 此处必须重新克隆initModel
        this.initModel = this.getInitModel();
        // this.$emit('update:modelValue', this.editModel);
    },
    render() {
        let editModel = this.editModel;
        let labelCol = this.$attrs.labelCol, wrapperCol = this.$attrs.wrapperCol;
        if(this.span instanceof Array) {
            labelCol = labelCol || {span: this.span[0]};
            wrapperCol = wrapperCol || {span: this.span[1]}
        }

        return (
            <AForm {...this.$attrs} model={editModel} labelCol={labelCol} wrapperCol={wrapperCol} ref="formRef">
                {this.$slots.default({model: editModel})}
            </AForm>)
    },
    methods: {
        // 元表单引用对象
        getFormRef() {
            return this.formRef;
        },
        validate() {
            return this.getFormRef().validate();
        },
        getRules() {
            return this.$attrs.rules;
        },
        getFormContext() {
            return this.formContext;
        },
        resetFields() {
            this.getFormRef().resetFields();
            // this.setEditModel(this.getInitModel())
        },
        scrollToField() {
            this.getFormRef().scrollToField();
        },

        validateFields() {
            return this.getFormRef().validateFields();
        },

        clearValidate() {
            this.getFormRef().clearValidate();
        },

        getInitModel() {
            // 此处必须用reactive声明为代理对象
            return reactive(SysUtils.clone(this.initModel));
        },

        getEditModel() {
            return this.editModel;
        },

        setEditModel(editModel) {
            if(!editModel) {
                throw new Error("UForm组件的model参数不能设置为null")
            }

            this.editModel = editModel;
            this.$emit('update:modelValue', editModel);
        }
    }
})
