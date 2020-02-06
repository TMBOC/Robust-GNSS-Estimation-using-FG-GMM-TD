function [std_t] = getError_new_TDPB(ref)

dff = ref(1:400,2:end) - mean(ref(1:400, 2:end));
for i = 1:length(dff)
    std_t(i) = sqrt(dff(i,:) * dff(i,:)');
end
