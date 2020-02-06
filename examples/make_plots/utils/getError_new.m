function [std_t] = getError_new(truth_t, ref)

% dff = ref(1:398,2:end) - mean(ref(1:398, 2:end));
dff = ref(1:400,2:end) - truth_t;
for i = 1:length(dff)
    std_t(i) = sqrt(dff(i,:) * dff(i,:)');
end
