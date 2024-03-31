BASEDIR=$(pwd)
echo -e "BASEDIR: $BASEDIR\n"

tf_init_up() {
    cd $1
    echo $(pwd)
    terraform init --upgrade >/dev/null
    echo "##### Done: $2 #####"
}

for cloud in */terraform/*; do
    tf_init_up $BASEDIR/$cloud $cloud &
done

wait
